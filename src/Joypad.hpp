#ifndef JOYPAD_H
#define JOYPAD_H

#include <linux/joystick.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include <string>

class Joypad {
private:
    int fd; // File descriptor
    struct js_event event;
    static const int MAX_AXES = 8;
    static const int MAX_BUTTONS = 12;
    static const int16_t MAX_AXIS_VALUE = 32767;  // Valor máximo do eixo no Linux
    
    int raw_axes[MAX_AXES];
    float normalized_axes[MAX_AXES];
    char buttons[MAX_BUTTONS];
    std::string devicePath;

    // Método privado para normalizar os valores dos eixos
    float normalizeAxisValue(int value) {
        return static_cast<float>(value) / MAX_AXIS_VALUE;
    }

public:
    Joypad(const std::string& path = "/dev/input/js0") : devicePath(path) {
        // Inicializa arrays
        memset(raw_axes, 0, sizeof(raw_axes));
        memset(normalized_axes, 0, sizeof(normalized_axes));
        memset(buttons, 0, sizeof(buttons));
        
        // Abre o dispositivo
        fd = open(devicePath.c_str(), O_RDONLY | O_NONBLOCK);
        if (fd == -1) {
            throw std::runtime_error("Não foi possível abrir o joystick: " + devicePath);
        }
    }
    
    ~Joypad() {
        if (fd != -1) {
            close(fd);
        }
    }
    
    bool readEvent() {
        ssize_t bytes = read(fd, &event, sizeof(event));
        
        if (bytes == sizeof(event)) {
            switch (event.type & ~JS_EVENT_INIT) {
                case JS_EVENT_AXIS:
                    if (event.number < MAX_AXES) {
                        raw_axes[event.number] = event.value;
                        normalized_axes[event.number] = normalizeAxisValue(event.value);
                    }
                    break;
                    
                case JS_EVENT_BUTTON:
                    if (event.number < MAX_BUTTONS) {
                        buttons[event.number] = event.value;
                    }
                    break;
            }
            return true;
        }
        return false;
    }
    
    // Getter para os valores dos eixos normalizados (-1 a 1)
    float getAxis(int axis) const {
        if (axis < 0 || axis >= MAX_AXES) {
            throw std::out_of_range("Índice do eixo inválido");
        }
        return normalized_axes[axis];
    }
    
    // Getter para os valores brutos dos eixos (opcional)
    int getRawAxis(int axis) const {
        if (axis < 0 || axis >= MAX_AXES) {
            throw std::out_of_range("Índice do eixo inválido");
        }
        return raw_axes[axis];
    }
    
    bool getButton(int button) const {
        if (button < 0 || button >= MAX_BUTTONS) {
            throw std::out_of_range("Índice do botão inválido");
        }
        return buttons[button] == 1;
    }
    
    bool isConnected() const {
        return fd != -1;
    }
};

#endif // JOYPAD_H


// int main() {
//     try {
//         Joypad joy;
        
//         while (true) {
//             while (joy.readEvent()) {
//                 // Lê os valores normalizados dos eixos
//                 printf("Eixo X: %.2f\n", joy.getAxis(0));  // Valores entre -1 e 1
//                 printf("Eixo Y: %.2f\n", joy.getAxis(1));  // Valores entre -1 e 1
                
//                 // Se precisar dos valores brutos
//                 // printf("Raw X: %d\n", joy.getRawAxis(0));
                
//                 if (joy.getButton(0)) {
//                     printf("Botão A pressionado!\n");
//                 }
//             }
//             usleep(1000);
//         }
//     } catch (const std::exception& e) {
//         std::cerr << "Erro: " << e.what() << std::endl;
//         return 1;
//     }
//     return 0;
// }