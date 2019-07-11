#include <Gui.h>
#include <Monitor.h>

#include <gtkmm/builder.h>

#include <iostream>

bool Gui::connected = false;
std::thread Gui::monitor;
std::filesystem::path Gui::_progpath;

Glib::Dispatcher Gui::disp_disconnect;
Glib::Dispatcher Gui::disp_outputs;
Glib::Dispatcher Gui::disp_vars;

Gtk::Window *Gui::window_main = nullptr;

Gtk::Button *Gui::button_connection = nullptr;

Gtk::Label *Gui::label_setpoint = nullptr;
Gtk::Label *Gui::label_presentvalue = nullptr;

Gtk::ProgressBar *Gui::progbar_tank = nullptr;

Gtk::Image *Gui::image_start = nullptr;
Gtk::Image *Gui::image_stop = nullptr;
Gtk::Image *Gui::image_reset = nullptr;

namespace fs = std::filesystem;

Gtk::Window* Gui::run(std::filesystem::path progpath)
{
    Glib::RefPtr<Gtk::Builder> builder;

    _progpath = progpath.parent_path().parent_path();

    //Open glade file
    try {
        builder = Gtk::Builder::create_from_file(fs::path(_progpath/"share/tank-plc-monitor/main.glade").string());
    } catch(...){
        throw;
    }
    
    //Get pointer to widgets of the main window
    builder->get_widget("window_main", window_main);
    if(!window_main)
        throw std::runtime_error("Unable to access window_main widget");
    
    builder->get_widget("button_connection", button_connection);
    if(!button_connection)
        throw std::runtime_error("Unable to access button_connection widget");

    builder->get_widget("label_setpoint", label_setpoint);
    if(!label_setpoint)
        throw std::runtime_error("Unable to access label_setpoint widget");

    builder->get_widget("label_presentvalue", label_presentvalue);
    if(!label_presentvalue)
        throw std::runtime_error("Unable to access label_presentvalue widget");

    builder->get_widget("progbar_tank", progbar_tank);
    if(!progbar_tank)
        throw std::runtime_error("Unable to access progbar_tank widget");

    builder->get_widget("image_start", image_start);
    if(!image_start)
        throw std::runtime_error("Unable to access image_start widget");
    
    builder->get_widget("image_stop", image_stop);
    if(!image_stop)
        throw std::runtime_error("Unable to access image_stop widget");

    builder->get_widget("image_reset", image_reset);
    if(!image_reset)
        throw std::runtime_error("Unable to access image_reset widget");

    //Signals of the main window
    button_connection->signal_clicked().connect(&Gui::on_button_connection_clicked);

    disp_disconnect.connect(&Gui::on_notification_disconnect);
    disp_outputs.connect(&Gui::on_notification_change_outputs);
    disp_vars.connect(&Gui::on_notification_change_vars);

    return window_main;
}

void Gui::on_button_connection_clicked()
{
    if(!connected){ // connect
        if(!Monitor::connect()){
            // show error dialog
            return;
        }
        connected = true;
        button_connection->set_label("gtk-disconnect");
        
        monitor = std::thread(Monitor::run);
    } else { // disconnect
        Monitor::disconnect(monitor);

        reset_gui();

        connected = false;
    }
}

void Gui::reset_gui()
{
    button_connection->set_label("gtk-connect");
    image_start->set(fs::path(_progpath/"share/tank-plc-monitor/light_off.png").string());
    image_stop->set(fs::path(_progpath/"share/tank-plc-monitor/light_off.png").string());
    image_reset->set(fs::path(_progpath/"share/tank-plc-monitor/light_off.png").string());
    progbar_tank->set_fraction(0);
    label_setpoint->set_text("0.0");
    label_presentvalue->set_text("0.0");
}

void Gui::notify_disconnect()
{
    disp_disconnect.emit();
}

void Gui::notify_change_outputs()
{
    disp_outputs.emit();
}

void Gui::notify_change_vars()
{
    disp_vars.emit();
}

void Gui::on_notification_disconnect()
{
    reset_gui();
    monitor.join();
    connected = false;
}

void Gui::on_notification_change_outputs()
{
    uint8_t lights = Monitor::get_outputs();
    if(Monitor::check_bit(lights, 0))
        image_start->set(fs::path(_progpath/"share/tank-plc-monitor/light_green.png").string());
    else
        image_start->set(fs::path(_progpath/"share/tank-plc-monitor/light_off.png").string());
    
    if(Monitor::check_bit(lights, 1))
        image_reset->set(fs::path(_progpath/"share/tank-plc-monitor/light_yellow.png").string());
    else
        image_reset->set(fs::path(_progpath/"share/tank-plc-monitor/light_off.png").string());

    if(Monitor::check_bit(lights, 2))
        image_stop->set(fs::path(_progpath/"share/tank-plc-monitor/light_red.png").string());
    else
        image_stop->set(fs::path(_progpath/"share/tank-plc-monitor/light_off.png").string());
}

void Gui::on_notification_change_vars()
{
    float vars[3];
    Monitor::get_vars(vars);

    char text[20];

    // std::cout << vars[0] << " " << vars[1] << " " << vars[2] << std::endl;
    
    progbar_tank->set_fraction((float)vars[0]/10.0); // percentage max 10 = 1 = 100%
    
    sprintf(text, "%.1f", (float)vars[2]);
    label_setpoint->set_text(text);
    
    sprintf(text, "%.1f", (float)vars[0]);
    label_presentvalue->set_text(text);
}