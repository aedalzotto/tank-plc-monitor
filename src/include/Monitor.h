#pragma once

#include <PLC.h>
#include <Gui.h>

#include <thread>
#include <mutex>

class Monitor {
public:
	static bool connect();
	static void disconnect(std::thread &handler);

	static void run(); // run(Gui &gui);

	static uint8_t get_outputs();
	static void get_vars(float *_vars);

	static bool check_bit(uint8_t outputs, uint8_t bit);
	static void shift_endianess(uint8_t *src, uint8_t *dst, uint8_t size);

private:
	static PLC s7_1200;

	static std::mutex mtx_keepalive;
	static std::mutex mtx_outputs;
	static std::mutex mtx_vars;
	static bool keepalive;

	static uint8_t outputs;
	static float vars[3];

	static bool start_light;
	static bool stop_light;
	static bool reset_light;

	//static float fill_value;
	//static float discharge_value;
	static float setpoint;
	static float presentvalue;

	static void stop();
};