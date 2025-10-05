#ifndef MENU_H
#define MENU_H

class Menu()
{
    private:
        int refresh_rate;
    public:
        virtual Page* create_page() const = 0;
        void set_refresh();
        
}

#endif

#ifndef MENU_PAGE_H
#define MENU_PAGE_H

enum class menu_page
{
    stats_page,
    visual_page,
    automation_page
};

class Stats_Page()
{
    private:
    public:
        Menu_Page();
}

#endif