#include <gtkmm/application.h>
#include <glibmm/fileutils.h>
#include <glibmm/markup.h>
#include <gtkmm/builder.h>

#include <Gui.h>
#include <Util.h>
#include <iostream>
#include <filesystem>
#include <Monitor.h>

namespace fs = std::filesystem;

int main(int argc, char *argv[])
{

    auto app = Gtk::Application::create(argc, argv, "com.github.aedalzotto.tank-plc-monitor");

    try {
        int ret = app->run(*Gui::run(std::filesystem::path(Util::System::get_program_path())));
        return ret;
    } catch(const Glib::FileError& ex){
        std::cerr << "FileError: " << ex.what() << std::endl;
        return 1;
    } catch(const Glib::MarkupError& ex){
        std::cerr << "MarkupError: " << ex.what() << std::endl;
        return 1;
    } catch(const Gtk::BuilderError& ex){
        std::cerr << "BuilderError: " << ex.what() << std::endl;
        return 1;
    } catch(const std::runtime_error& ex){
        std::cerr << "BuilderError: " << ex.what() << std::endl;
        return 1;
    } catch(const std::exception &ex){
        std::cerr << ex.what() << std::endl;
		return 1;
    }

}