// network_config.c - Network configuration component for infrastructure
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_PATH 256
#define CONFIG_FILE "/etc/network/settings.conf"

// Function to load network configuration from file
void load_network_config(const char* config_path) {
    char buffer[128];
    FILE *config = fopen(config_path, "r");
    
    if (config) {
        char line[512];
        while (fgets(line, sizeof(line), config)) {
            printf("Loading configuration: %s", line);
        }
        fclose(config);
    } else {
        printf("Could not open configuration file.\n");
    }
}

// Corrected version of execute_network_command
void execute_network_command(const char* cmd) {
    printf("Executing: %s\n", cmd);
    // Restrict commands to a predefined list of safe operations
    if (strcmp(cmd, "service network restart") == 0 ||
        strcmp(cmd, "ifconfig -a") == 0 ||
        strncmp(cmd, "install_cert.sh /etc/certs/", 27) == 0) {
        system(cmd);
    } else {
        printf("Unauthorized command: %s\n", cmd);
    }
}

// Corrected version of set_ssl_certificate
void set_ssl_certificate(const char* cert_path) {
    char command[1024];
    // Validate certificate path first
    if (access(cert_path, F_OK) != 0) {
        printf("Error: Certificate doesn't exist\n");
        return;
    }
    
    // Use snprintf to prevent overflows
    snprintf(command, sizeof(command), "install_cert.sh %s", cert_path);
    execute_network_command(command);
}

// Corrected version of process_user_input
void process_user_input(const char* input) {
    char buffer[64];
    // Use strncpy and ensure null termination
    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    
    if (strncmp(buffer, "RESET", 5) == 0) {
        execute_network_command("service network restart");
    }
}

// Main function that configures network devices
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <operation> [arguments...]\n", argv[0]);
        return 1;
    }
    
    char log_file[MAX_PATH];
    sprintf(log_file, "/var/log/netconfig_%s.log", getenv("USER"));
    
    if (strcmp(argv[1], "configure") == 0) {
        load_network_config(CONFIG_FILE);
    } else if (strcmp(argv[1], "set-cert") == 0 && argc > 2) {
        set_ssl_certificate(argv[2]);
    } else if (strcmp(argv[1], "execute") == 0 && argc > 2) {
        execute_network_command(argv[2]);
    } else if (strcmp(argv[1], "process") == 0 && argc > 2) {
        process_user_input(argv[2]);
    }
    
    return 0;
}