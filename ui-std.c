#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include "plcemu.h"
#include "ui.h"

/*************GLOBALS************************************************/

int Enable = FALSE;

void ui_display_message(char * msgstr)
{
    printf("%s\n", msgstr);
}

void init_help()
{
	FILE * f;
    char line[MAXSTR], helpline[MAXSTR];
    if ((f = fopen("./help", "r")))
	{
		while (fgets(line, MEDSTR, f))
         //read help file
            printf(helpline, " %s\n", line);
		fclose(f);
	}
}

void time_header()
{
	char t[TINYSTR], *p;
	char str[SMALLSTR];
	time_t now;

	time(&now);
	strcpy(t, ctime(&now));
	t[19] = '\0';
	p = t + 10;
    printf(str, " PLC-EMUlator v%4.2f %14s\n ", PRINTABLE_VERSION, p);
}


void ui_draw()
{
    char str[SMALLSTR];
    if (plc.status % 2) //running
        printf(str, "Hardware:%s  RUNNING\n", Hw);
    else
        printf(str, "Hardware:%s  STOPPED\n", Hw);
//	draw_info_line(1,"F1/2:Force 1/0|F3:Unforce|F4:Run|F5:Edit|F6:Lock|F7:Load|FBYTESIZE:Save|F9:Help|F10:Quit");


}

int ui_init(int screenW)
{
    init_help();
    return 1;
}

int ui_update(int page)
{
    int nextpage=0;
    time_header();

    return nextpage;
}

void ui_end()
{
    return;
}

void ui_toggle_enabled()
{
    Enable = Enable ? 0 : 1;
}
