
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define DEVICE "/dev/usb_driver_dev"
#define BUFFER_LENGTH 80

int main() {
	int i, fd, bytes;
	char ch, write_buf[BUFFER_LENGTH], read_buf[BUFFER_LENGTH];
	
	fd = open(DEVICE, O_RDWR);
	if (fd == -1) {
		printf("Error occured while opening the device %s\n", DEVICE);
		exit(-1);
	}
	
	printf("r = read from the device\nw = write to device\nenter a command: ");
	scanf("%c", &ch);
	
	switch (ch) {
		case 'w':
			printf("Enter data: ");
			scanf(" %[^\n]", write_buf);
			write(fd, write_buf, BUFFER_LENGTH);
			break;
		case 'r':
			bytes = read(fd, read_buf, BUFFER_LENGTH);
			if (bytes > 0) {
				for (int i = 0; i < bytes; ++i) {
					printf("%c", read_buf[i]);
				}
			}
			printf("\n");
			break;
		default:
			printf("wrong command\n");
			break;
	}
	close(fd);
	
	return 0;
	
}
