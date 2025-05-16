#include <gtkmm.h>
#include <iostream>
#include <cstdio>
#include <boost/program_options.hpp>

using namespace Glib;
using namespace Gtk;

class MyWindow : public Window
{
private:
    Gtk::Box            vbox{Gtk::ORIENTATION_VERTICAL};
    Gtk::MenuBar        menubar;
    Gtk::Statusbar      statusbar;

    Fixed               fixed;
    ScrolledWindow      scrolledWindow;
    Entry               textBox1;
    Entry               textBox2;
    Label               label1;

    TreeModelColumn< int >              m_col_num;
    TreeModelColumn< Glib::ustring >    m_col_name;
    TreeModel::ColumnRecord             m_record;
    Glib::RefPtr< Gtk::ListStore >      m_liststore;
    ScrolledWindow      scrolledWindow2;
    TreeView            m_treeview;

    Notebook            tabControl1;
    Frame               tabPage1;
    Frame               tabPage2;
    Frame               tabPage3;

public:
    MyWindow()
    {
        /* attachment of widget */
        vbox.pack_start(menubar, Gtk::PACK_SHRINK);
        vbox.pack_start(tabControl1 );
        vbox.pack_end(statusbar, Gtk::PACK_SHRINK);
        add(vbox);

        /* create Menu Bar */
        auto file_menuitem = Gtk::make_managed<Gtk::MenuItem>("_File", true);
        auto quit_item = Gtk::make_managed<Gtk::MenuItem>("_Quit", true);
        auto file_menu = Gtk::make_managed<Gtk::Menu>();
        quit_item->signal_activate().connect(sigc::mem_fun(*this, &MyWindow::on_menu_quit));
        file_menu->append(*quit_item);
        file_menuitem->set_submenu(*file_menu);
        menubar.append(*file_menuitem);

        /* Status Bar */
        statusbar.push("準備完了");

        /* create TAB Control Panel */
        tabControl1.insert_page( tabPage1, "tabPage1", 0 );
        tabControl1.insert_page( tabPage2, "tabPage2", 1 );
        tabControl1.insert_page( tabPage3, "tabPage3", 2 );

        tabPage1.add( scrolledWindow );
        scrolledWindow.add( fixed );
        textBox1.set_text("textBox1");
        fixed.add(textBox1);
        fixed.move(textBox1, 10, 10);

        label1.set_text( "Hello, World!" );
        label1.override_color( Gdk::RGBA( "#008000" ) );
        Pango::FontDescription textFont;
        textFont.set_family( "Arial" );
        textFont.set_size( 34 * PANGO_SCALE );
        textFont.set_style( Pango::Style::STYLE_ITALIC );
        textFont.set_weight( Pango::Weight::WEIGHT_BOLD );
        label1.override_font( textFont );
        fixed.add( label1 );
        fixed.move( label1, 10, 100 );

        tabPage2.add( textBox2 );
        textBox2.set_text("textBox2");

        m_record.add( m_col_num );
        m_record.add( m_col_name );
        m_liststore = Gtk::ListStore::create( m_record );
        m_treeview.set_model( m_liststore );
        m_treeview.append_column( "番号", m_col_num );
        m_treeview.append_column( "名前", m_col_name );
        TreeModel::Row row;
        for( int i = 0; i < 10; ++i )
        {
            row = *( m_liststore->append() );
            row[ m_col_num ] = i;
            row[ m_col_name ] = "ほげ";
        }
        scrolledWindow2.add(m_treeview);
        tabPage3.add( scrolledWindow2 );

        set_title( "My first application" );
        resize( (320 * 2), (240 * 2));
        show_all();
    }
    ~MyWindow(void) { }

protected:
    void on_menu_quit()
    {
        hide();
    }
};

int main(int argc, char* argv[])
{
    try
    {
        boost::program_options::options_description desc("hello.exe [Options]");
        desc.add_options()
            ("version,v", "Print version")
            ("help,h",    "help");
        boost::program_options::variables_map argmap;
        auto const parsing_result = parse_command_line(argc, argv, desc);
        store(parsing_result, argmap);
        notify(argmap);
        if(argmap.count("version"))
        {
            printf("Version 0.0.1\n");
            return (1);
        }
        if(argmap.count("help"))
        {
            std::cout << desc << std::endl;
            return (2);
        }
        RefPtr<Application> application = Application::create(argc, argv);
        MyWindow win;
        return application->run(win);
    }
    catch(const std::exception & e) { std::cout << "exeption: "       << e.what() << std::endl; }
    catch(...)                      { std::cout << "unknown exeption"             << std::endl; }
    return(-1);
}