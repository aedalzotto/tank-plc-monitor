#include <Monitor.h>
#include <PLC.h>

#include <cmath>

PLC Monitor::s7_1200("IF1");

std::mutex Monitor::mtx_keepalive;
std::mutex Monitor::mtx_outputs;
std::mutex Monitor::mtx_vars;
bool Monitor::keepalive = true;

uint8_t Monitor::outputs;
float Monitor::vars[3];

bool Monitor::start_light = false;
bool Monitor::stop_light = false;
bool Monitor::reset_light = false;

//float Monitor::fill_value = 0;
//float Monitor::discharge_value = 0;
float Monitor::setpoint = 0;
float Monitor::presentvalue = 0;

bool Monitor::connect()
{
	return s7_1200.connect_tcp("192.168.0.7", 102);
}

void Monitor::run()
{
	int ret = 0;
	bool stop = false;

	mtx_outputs.lock();
	start_light = false;
	stop_light = false;
	reset_light = false;
	mtx_outputs.unlock();

	mtx_vars.lock();
	//fill_value = 0;
	//discharge_value = 0;
	setpoint = 0;
	presentvalue = 0;
	mtx_vars.unlock();

	mtx_keepalive.lock();
	keepalive = true;
	mtx_keepalive.unlock();

	while(!stop){
		// Monitor
		mtx_outputs.lock();
		ret |= s7_1200.read_bytes(PLC::DaveMemArea::OUTPUTS, 0, 1, &outputs);
		mtx_outputs.unlock();

		// ID100 & ID108
		mtx_vars.lock();
		float tmpvars[3];
		ret |= s7_1200.read_bytes(PLC::DaveMemArea::INPUTS, 100, 12, (uint8_t*)tmpvars);
		shift_endianess((uint8_t*)(&tmpvars[0]), (uint8_t*)&vars[0], sizeof(float));
		shift_endianess((uint8_t*)(&tmpvars[1]), (uint8_t*)&vars[1], sizeof(float));
		shift_endianess((uint8_t*)(&tmpvars[2]), (uint8_t*)&vars[2], sizeof(float));

		mtx_vars.unlock();

		if(ret){
			Gui::notify_disconnect();
			return;
		}

		// std::cout << (int)outputs << std::endl;

		mtx_outputs.lock();
		bool state = check_bit(outputs, 0);
		bool changed = false;
		if(start_light != state){
			start_light = state;
			changed = true;
		}

		state = check_bit(outputs, 1);
		if(reset_light != state){
			reset_light = state;
			changed = true;
		}

		state = check_bit(outputs, 2);
		if(stop_light != state){
			stop_light = state;
			changed = true;
		}
		mtx_outputs.unlock();

		if(changed)
			Gui::notify_change_outputs();

		mtx_vars.lock();
		changed = false;

		// trucate with 1 decimal point
		// Level meter
		if((int)(round(vars[0]*10)) != (int)(round(presentvalue*10))){
			presentvalue = vars[0];
			changed = true;
		}

		// Setpoint (endianness inverte)
		if((int)(round(vars[2]*10)) != (int)(round(setpoint*10))){
			setpoint = vars[2];
			changed = true;
		}		

		mtx_vars.unlock();

		if(changed)
			Gui::notify_change_vars();
		
		std::this_thread::sleep_for (std::chrono::milliseconds(250));

		mtx_keepalive.lock();
		stop = !keepalive;
		mtx_keepalive.unlock();
	}
}

void Monitor::disconnect(std::thread &handler)
{
	stop();
    handler.join();
    s7_1200.disconnect();
}

void Monitor::stop()
{
	std::lock_guard<std::mutex> lock(mtx_keepalive);
	keepalive = false;
}

bool Monitor::check_bit(uint8_t outputs, uint8_t bit)
{
	return ((outputs & (1 << bit)) >> bit);
}

uint8_t Monitor::get_outputs()
{
	std::lock_guard<std::mutex> lock(mtx_outputs);
	return outputs;
}

void Monitor::get_vars(float *_vars)
{
	std::lock_guard<std::mutex> lock(mtx_vars);
	memcpy(_vars, vars, 3*sizeof(float));
}

void Monitor::shift_endianess(uint8_t *src, uint8_t *dst, uint8_t size)
{
	for(uint8_t i = 0; i < size; i++)
		dst[i] = src[size-1-i];
}