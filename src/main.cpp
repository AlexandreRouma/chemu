#include <SDL2/SDL.h>
#undef main
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "command_args.h"
#include "lcd.h"
#include <mutex>
#include <Windows.h>

std::mutex lcdMtx;

inline uint8_t hexDecode(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 0xA;
    }
    else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 0xA;
    }
    return 0xFF;
}

void serialWorker(std::string port, LCD* lcd) {
    // Open port
    HANDLE iface = CreateFileA(
        "\\\\.\\COM16", // TODO: Use port
        GENERIC_READ | GENERIC_WRITE,
        0,
        0,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        0
    );

    // Configure port
    DCB dcb = {0};
    dcb.DCBlength=sizeof(dcb);
    if (!GetCommState(iface, &dcb)) {
        printf("Could not get port config\n");
        return;
    }
    dcb.BaudRate=CBR_115200;
    dcb.ByteSize=8;
    dcb.Parity=NOPARITY;
    dcb.StopBits=ONESTOPBIT;
    if(!SetCommState(iface, &dcb)){
        printf("Could not set port config\n");
        return;
    }

    printf("Started\n");

    std::string line;
    while (true) {
        // Read a line
        line.clear();
        char c;
        DWORD bread = 0;
        while (true) {
            if (!ReadFile(iface, &c, 1, &bread, NULL)) {
                printf("Some fucking shit happened: %d\n", GetLastError());
                break;
            }
            if (c == '\n' || bread <= 0) { break; }
            if (bread) { line += c; }
            else {
                printf("Read failed\n");
            }
        }

        // If error, break out
        if (bread <= 0) {
            printf("Read failed: %d\n", bread);
            break;
        }

        // Extract bytes
        bool nextEscaped = false;
        std::vector<uint8_t> packet;
        for (int i = 18; i < line.size(); i += 2) {
            printf("%c%c", line[i], line[i+1]);

            // Break if an unexpected char is present
            if (!std::isxdigit(line[i]) || !std::isxdigit(line[i+1])) { break; }

            // Decode byte
            uint8_t byte = (hexDecode(line[i]) << 4) | hexDecode(line[i+1]);

            // If the last byte was escaped, write the unescaped version
            if (nextEscaped) {
                packet.push_back(byte ^ (1 << 5));
                nextEscaped = false;
                continue;
            }

            // Check for escapement
            if (byte == 0x7D) {
                nextEscaped = true;
                continue;
            }

            // Append byte
            packet.push_back(byte);
        }
        printf("\n");

        if (line[0] == '>' && packet[0] == 0xFA && packet[1] == 0x08 && packet[2] == 0x80) {
            int page = packet[3] % 4;
            int col = packet[4] & 0x7F;
            {
                std::lock_guard lck(lcdMtx);
                lcd->blit(page, col, &(packet.data())[5], packet.size() - 7);
            }
        }
        else if (line[0] == '>') {
            printf("RF -> HS: ");
            for (int i = 0; i < packet.size() - 2; i++) {
                printf("%02X ", packet[i]);
            }
            printf("\n");
        }
        else {
            printf("HS -> RF: ");
            for (int i = 0; i < packet.size() - 2; i++) {
                printf("%02X ", packet[i]);
            }
            printf("\n");
        }
    }
}

int main(int argc, char* argv[]) {
    // Parse arguments
    CommandArgsParser cargs;
    cargs.define('c', "com", "Serial port", "");
    cargs.parse(argc, argv);

    // Check that a rom was given
    if (cargs["com"].s().empty()) {
        fprintf(stderr, "No COM port given!\n");
        return -1;
    }

    // Init LCD
    LCD lcd(98, 32, 11, 1, 24, 0xFF8DCD05, 0xFF78B801, 0xFF246300);
    int fbWidth = lcd.getFBWidth();
    int fbHeight = lcd.getFBHeight();

    // Init SDL
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* win = SDL_CreateWindow("CODAN Handset Emulator", 100, 100, fbWidth, fbHeight, SDL_WINDOW_SHOWN);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_Texture * tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, fbWidth, fbHeight);

    // Start serial worker
    std::thread workerThread(serialWorker, cargs["com"].s(), &lcd);

    // Mais loop
    uint64_t lastTime = SDL_GetTicks();
    bool running = true;
    int frame = 0;
    while (running) {
        // Check for any event
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EventType::SDL_KEYDOWN) {
                // TODO
            }
            else if (event.type == SDL_EventType::SDL_KEYUP) {
                // TODO
            }
            else if (event.type == SDL_EventType::SDL_QUIT) {
                running = false;
            }
        }

        // Draw framebuffer
        {
            std::lock_guard lck(lcdMtx);
            SDL_UpdateTexture(tex, NULL, lcd.getFB(), fbWidth * sizeof(uint32_t));
        }
        
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, NULL, NULL);
        SDL_RenderPresent(ren);
    }

    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}