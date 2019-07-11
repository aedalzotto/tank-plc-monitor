#pragma once

#include <filesystem>
#include <thread>

#include <gtkmm/window.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/image.h>
#include <glibmm/dispatcher.h>

class Gui {
public:
    static Gtk::Window* run(std::filesystem::path progpath);

    static void notify_disconnect();
    static void notify_change_outputs();
    static void notify_change_vars();

private:
    static bool connected;
    static std::thread monitor;
    static std::filesystem::path _progpath;

    static Glib::Dispatcher disp_disconnect;
    static Glib::Dispatcher disp_outputs;
    static Glib::Dispatcher disp_vars;

    static Gtk::Window *window_main;

    static Gtk::Button *button_connection;

    static Gtk::Label *label_setpoint;
    static Gtk::Label *label_presentvalue;

    static Gtk::ProgressBar *progbar_tank;

    static Gtk::Image *image_start;
    static Gtk::Image *image_stop;
    static Gtk::Image *image_reset;

    static void reset_gui();

    static void on_button_connection_clicked();

    static void on_notification_disconnect();
    static void on_notification_change_outputs();
    static void on_notification_change_vars();

};