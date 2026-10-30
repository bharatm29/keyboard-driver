#include <stdio.h> // printf
#include <sys/types.h>
#include <wchar.h> // wchar_t

#include <hidapi/hidapi.h>

#define MAX_STR 255

static hid_device *keyboard;

void print_device(struct hid_device_info *cur_dev) {
	printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
	printf("\n");
	printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
	printf("  Product:      %ls\n", cur_dev->product_string);
	printf("  Release:      %hx\n", cur_dev->release_number);
	printf("  Interface:    %d\n",  cur_dev->interface_number);
	printf("  Usage (page): 0x%hx (0x%hx)\n", cur_dev->usage, cur_dev->usage_page);
	printf("\n");
}

void print_hid_report_descriptor_from_device(hid_device *device) {
	unsigned char descriptor[HID_API_MAX_REPORT_DESCRIPTOR_SIZE];
	int res = 0;

	printf("  Report Descriptor: ");
#if HID_API_VERSION >= HID_API_MAKE_VERSION(0, 14, 0)
	res = hid_get_report_descriptor(device, descriptor, sizeof(descriptor));
#else
	(void)res;
#endif
	if (res < 0) {
		printf("error getting: %ls", hid_error(device));
	}
	else {
		printf("(%d bytes)", res);
	}
	for (int i = 0; i < res; i++) {
		if (i % 10 == 0) {
			printf("\n");
		}
		printf("0x%02x, ", descriptor[i]);
	}
	printf("\n");
}

void print_hid_report_descriptor_from_path(const char *path) {
	hid_device *device = hid_open_path(path);
	if (device) {
		print_hid_report_descriptor_from_device(device);
		hid_close(device);
	}
	else {
		printf("  Report Descriptor: Unable to open device by path\n");
	}
}

void print_devices(struct hid_device_info *cur_dev) {
	for (; cur_dev; cur_dev = cur_dev->next) {
		print_device(cur_dev);
	}
}

void print_devices_with_descriptor(struct hid_device_info *cur_dev) {
	for (; cur_dev; cur_dev = cur_dev->next) {
		print_device(cur_dev);
		print_hid_report_descriptor_from_path(cur_dev->path);
	}
}

void setup_hid() {
    wchar_t wstr[MAX_STR];
    int res;
    int i;

    res = hid_init();
    if (res < 0) {
        printf("Init failed: %ls\n", hid_error(NULL));
        hid_exit();
    }

	// struct hid_device_info* devs = hid_enumerate(0x0, 0x0);
	// print_devices_with_descriptor(devs);
	// hid_free_enumeration(devs);

    keyboard = hid_open_path("/dev/hidraw2");
    if (!keyboard) {
        printf("Unable to open device: %ls\n", hid_error(keyboard));
        hid_exit();
    }

    res = hid_get_manufacturer_string(keyboard, wstr, MAX_STR);
    printf("Manufacturer String: %ls\n", wstr);

    res = hid_get_product_string(keyboard, wstr, MAX_STR);
    printf("Product String: %ls\n", wstr);
}

void cmd(u_char mode, u_char brightness, u_char animation, u_char sleep, u_char colormixing, u_char r, u_char g, u_char b) {
    unsigned char buf[65] = {
    0x0a, // report ID -> same everytime
        0x01, 0x01, 0x02, 0x29,
        mode, // MODE
        0x00, animation, // Animation speed
        brightness, // Brightness (1 - 5)
        r, g, b, // (r, g, b)
        colormixing, // Color mixing (00 | 01)
        sleep // Sleep time (01 to 05 -> 5, 10, 20, 30, NO)
    };

    int res = hid_send_feature_report(keyboard, buf, 65);
    if (res < 0) {
        printf("Feature request failed: %ls\n", hid_error(keyboard));
        hid_exit();
    } else {
        printf("Sent successfully: %d\n", res);
    }

    // for(int i = 0; i < res; i++) {
    //     if (res / 10 == 0) {
    //         printf("\n");
    //     }
    //     printf("%.2x ", buf[i]);
    // }
    // printf("\n");
}

void close_hid() {
    // Close the device
    hid_close(keyboard);

    // Finalize the hidapi library
    hid_exit();
}
