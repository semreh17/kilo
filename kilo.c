/*** includes ***/
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>


/*** data structures ***/
struct termios orig_termios;


/*** terminal functions  ***/ 
void die(const char *s) {
	perror(s);
	exit(1);
}

void disable_raw_mode() {
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
		die("tcsettattr");
}

/* this funciont changes the terminal mode, turning it from cooked to raw */
void enable_raw_mode() {

	if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgettattr");
	atexit(disable_raw_mode);

	struct termios raw = orig_termios;

	// all consts are present in the termios man
	// CS8, ISTRIP, INPCK e BKRINP are less important and are there just for
	// specific and small cases
	raw.c_iflag &= ~(IXON | ICRNL | INPCK | BRKINT | ISTRIP);	
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag |= (CS8);
	raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
	
	// min and max values to handle the time out for the read function
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;


	// applying the modified flag to the terminal
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsettattr");
			
}
/*** init ***/
int main() {
	enable_raw_mode();
	
	while (1) {
		char c = '\0';
		// in cygwin read returns -1 and EAGAIN in errno when it times out
		// thats why we add errno != EAGAIN
		if(read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
	
		if (iscntrl(c)) {
			printf("%d\r\n", c);
		} else {
			printf("%d ('%c')\r\n", c, c);
		}
		if (c == 'q') break;
	}
			
	return 0;
}	
