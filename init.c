#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct{
	char* path;
	bool daemon;
	char* args[9];
} command;

void fork_exec(char* path, bool daemon, char* args[]);

int main(){
	command commands[] = {
		//input settings
		{"/usr/bin/riverctl", false, {"riverctl", "input", "pointer-2-14-ETPS/2_Elantech_Touchpad", "events", "disabled", NULL}},
		{"/usr/bin/riverctl", false, {"riverctl", "keyboard-layout", "-options", "grp:alt_shift_toggle", "us,ru", NULL}},
		{"/usr/bin/riverctl", false, {"riverctl", "set-repeat", "50", "300", NULL}},

		//run audio server
		{"/usr/bin/pipewire", true, {"pipewire", NULL}},
		{"/usr/bin/pipewire-pulse", true, {"pipewire-pulse", NULL}},
		{"/usr/bin/wireplumber", true, {"wireplumber", NULL}},

		//run bar
		{"/usr/bin/waybar", true, {"waybar", NULL}},

		//setup keymaps		
		{"/usr/bin/riverctl", false, {"riverctl", "map", "normal", "Super", "Return", "spawn", "exec kitty", NULL}},
		{"/usr/bin/riverctl", false, {"riverctl", "map", "normal", "Super", "X", "spawn", "exec firefox", NULL}},
		{"/usr/bin/riverctl", false, {"riverctl", "map", "normal", "Super", "D", "spawn", "exec dmenu-wl_run -nf '#bfbfbf' -nb '#000000' -sf '#cfcfcf' -sb '#252525'", NULL}},

		{"/usr/bin/riverctl", false, {"riverctl", "map", "normal", "Super", "Q", "close", NULL}},
		{"/usr/bin/riverctl", false, {"riverctl", "map", "normal", "Super+Shift", "Q", "exit", NULL}},

		{"/usr/bin/riverctl", false, {"riverctl", "map", "normal", "Super", "Right", "focus-view", "right", NULL}},
		{"/usr/bin/riverctl", false, {"riverctl", "map", "normal", "Super", "Left", "focus-view", "left", NULL}},
		{"/usr/bin/riverctl", false, {"riverctl", "map", "normal", "Super", "Up", "focus-view", "up", NULL}},
		{"/usr/bin/riverctl", false, {"riverctl", "map", "normal", "Super", "Down", "focus-view", "down", NULL}},

		{"/usr/bin/riverctl", false, {"riverctl", "map", "normal", "Super+Shift", "Right", "swap", "right", NULL}},
		{"/usr/bin/riverctl", false, {"riverctl", "map", "normal", "Super+Shift", "Left", "swap", "left", NULL}},
		{"/usr/bin/riverctl", false, {"riverctl", "map", "normal", "Super+Shift", "Up", "swap", "up", NULL}},
		{"/usr/bin/riverctl", false, {"riverctl", "map", "normal", "Super+Shift", "Down", "swap", "down", NULL}},

		{"/usr/bin/riverctl", false, {"riverctl", "map", "normal", "Super+Control", "Right", "send-layout-cmd", "rivertile", "main-ratio +0.05", NULL}},
		{"/usr/bin/riverctl", false, {"riverctl", "map", "normal", "Super+Control", "Left", "send-layout-cmd", "rivertile", "main-ratio -0.05", NULL}},

		{"/usr/bin/riverctl", false, {"riverctl", "map", "normal", "Super+Alt", "Right", "send-layout-cmd", "rivertile", "main-count +1", NULL}},
		{"/usr/bin/riverctl", false, {"riverctl", "map", "normal", "Super+Alt", "Left", "send-layout-cmd", "rivertile", "main-count -1", NULL}},

		{"/usr/bin/riverctl", false, {"riverctl", "map", "normal", "Super", "Space", "toggle-float", NULL}},
		{"/usr/bin/riverctl", false, {"riverctl", "map", "normal", "Super", "F", "toggle-fullscreen", NULL}},

		//setup mouse keymaps
		{"/usr/bin/riverctl", false, {"riverctl", "map-pointer", "normal", "Super", "BTN_LEFT", "move-view", NULL}},
		{"/usr/bin/riverctl", false, {"riverctl", "map-pointer", "normal", "Super", "BTN_RIGHT", "resize-view", NULL}},
		{"/usr/bin/riverctl", false, {"riverctl", "map-pointer", "normal", "Super", "BTN_MIDDLE", "toggle-float", NULL}},

		//setup basic appearance
		{"/usr/bin/riverctl", false, {"riverctl", "background-color", "0x000000", NULL}},
		{"/usr/bin/riverctl", false, {"riverctl", "border-color-focused", "0x555555", NULL}},
		{"/usr/bin/riverctl", false, {"riverctl", "border-color-unfocused", "0x222222", NULL}},
		{"/usr/bin/riverctl", false, {"riverctl", "border-width", "1", NULL}},

		//run wallpaper daemon
		{"/usr/bin/swww-daemon", true, {"swww-daemon", NULL}},

		//rules
		{"/usr/bin/riverctl", false, {"riverctl", "rule-add", "ssd", NULL}},

		//start rivertile
		{"/usr/bin/riverctl", false, {"riverctl", "default-layout", "rivertile", NULL}},
		{"/usr/bin/rivertile", true, {"rivertile", "-view-padding", "0", "-outer-padding", "0", "-main-ratio", "0.5", NULL}},

		//part of config that have not rewroten yet
		{"/bin/bash", false, {"bash", "-c", ".config/river/init.sh", NULL}}
	};

	const int count = sizeof(commands)/sizeof(commands[0]);

	for(int i = 0; i < count; i++)
		fork_exec(commands[i].path, commands[i].daemon, commands[i].args);
	
	for(int i = 0; i < count; i++)
		wait(NULL);

	return 0;
}

void daemonize();

void fork_exec(char* path, bool daemon, char* args[]) {
	pid_t pid = fork();

	if (pid == 0){
		if(daemon){
			daemonize();
		}

		execv(path, args);
		fprintf(stderr, "Exec Error. Can not run %s.\n", path);
		exit(1);
	}
	else if (pid < 0) {
		fprintf(stderr, "Fork Error. Can not create child process.\n");
		exit(1);
	}
}

void daemonize(){
	pid_t pid = fork();

	if(pid > 0){
		exit(0);
	}
	else if(pid < 0){
		fprintf(stderr, "Fork Error. Can not create child process.\n");
		exit(1);
	}

	if(setsid() < 0){
		fprintf(stderr, "Setsid Error. Can not create new session.\n");
		exit(1);
	}
}
