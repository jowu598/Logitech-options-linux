#include <assert.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

// TODO(jowu): Get vendor of keyboard from evdev.
//
void show_usage() {
    printf("usage: \n-h help\n");
    printf(
        "sudo ./logi-key-emulator /dev/input/mouse-of-logitech "
        "/dev/input/your-keyborad \n");
    printf("suggest to use evtest checkout the inputs.\n");
}

void write_event_to_keyboard(int fd, int type, int code, int value) {
    struct input_event evt;
    evt.type = type;
    evt.code = code;
    evt.value = value;
    write(fd, &evt, sizeof(struct input_event));
}

void Run(const char* mouse_dev, const char* key_dev) {
    int rfd = open(mouse_dev, O_RDONLY);
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(rfd, &fds);
    int wfd = open(key_dev, O_RDWR);
    assert(wfd != -1);
    while (1) {
        select(rfd + 1, &fds, NULL, NULL, NULL);
        struct input_event evt;
        int rd = read(rfd, &evt, sizeof(struct input_event));
        if (rd > 0) {
            if (evt.type == 2 && evt.code == 6) {
                if (evt.value == -1) {
                    // Simulate a ctrl+down key event.
                    // Ctrl enter.
                    write_event_to_keyboard(wfd, 4, 4, 0x1d);
                    write_event_to_keyboard(wfd, 1, 29, 1);
                    // Page down enter.
                    write_event_to_keyboard(wfd, 4, 4, 0xc9);
                    write_event_to_keyboard(wfd, 1, 104, 1);
                    // Page down leave.
                    write_event_to_keyboard(wfd, 4, 4, 0xc9);
                    write_event_to_keyboard(wfd, 1, 104, 0);
                    // Ctrl leave.
                    write_event_to_keyboard(wfd, 4, 4, 0x1d);
                    write_event_to_keyboard(wfd, 1, 29, 0);
                } else if (evt.value == 1) {
                    // Ctrl enter.
                    write_event_to_keyboard(wfd, 4, 4, 0xc9);
                    write_event_to_keyboard(wfd, 1, 29, 1);
                    // Page up enter.
                    write_event_to_keyboard(wfd, 4, 4, 0xc9);
                    write_event_to_keyboard(wfd, 1, 109, 1);
                    // Page up leave.
                    write_event_to_keyboard(wfd, 4, 4, 0xc9);
                    write_event_to_keyboard(wfd, 1, 109, 0);
                    // Ctrl leave.
                    write_event_to_keyboard(wfd, 4, 4, 0x1d);
                    write_event_to_keyboard(wfd, 1, 29, 0);
                } else {
                }
            }
        }
    }

    close(rfd);
    close(wfd);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        show_usage();
        return -1;
    }

    char* mouse_dev = argv[1];
    char* key_dev = argv[2];
    Run(mouse_dev, key_dev);
    return 0;
}
