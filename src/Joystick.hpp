#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <linux/joystick.h>
#include <sys/select.h>

class Joystick {
public:
    Joystick(const std::string& device = "/dev/input/js0") {
        // Abre o dispositivo de joystick
        fd = open(device.c_str(), O_RDONLY);
        if (fd == -1) {
            std::cerr << "Erro ao abrir o dispositivo: " << device << std::endl;
            return;
        }
        
        // Pega o nome do joystick
        if (ioctl(fd, JSIOCGNAME(128), name) < 0) {
            std::cerr << "Erro ao obter o nome do joystick" << std::endl;
            return;
        }
        
        std::cout << "Joystick: " << name << " aberto com sucesso!" << std::endl;
    }

    ~Joystick() {
        if (fd != -1) {
            close(fd);
            std::cout << "Joystick fechado." << std::endl;
        }
    }

    bool isValid() const {
        return fd != -1;
    }

    // Lê um evento de entrada do joystick com select()
    bool readEvent() {
        struct js_event event;
        
        // Usamos select para esperar por eventos
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        struct timeval tv = {0, 0};  // Não bloqueante

        int ret = select(fd + 1, &fds, nullptr, nullptr, &tv);
        if (ret == -1) {
            std::cerr << "Erro no select!" << std::endl;
            return false;
        }

        if (ret > 0 && FD_ISSET(fd, &fds)) {
            ssize_t bytesRead = read(fd, &event, sizeof(event));

            if (bytesRead == -1) {
                std::cerr << "Erro ao ler evento do joystick!" << std::endl;
                return false;
            }

            if (bytesRead == sizeof(event)) {
                // Processa o evento
                switch (event.type) {
                    case JS_EVENT_AXIS:
                        // Normaliza o valor do eixo para o intervalo [-1, 1]
                        float normalizedAxis = event.value / 32768.0f;
                        std::cout << "Eixo " << (int)event.number << ": " << normalizedAxis << std::endl;
                        break;
                    case JS_EVENT_BUTTON:
                        std::cout << "Botão " << (int)event.number
                                  << " " << (event.value ? "pressionado" : "liberado") << std::endl;
                        
                        // Se o botão 0 for pressionado, saímos do loop
                        if (event.number == 0 && event.value == 1) {
                            std::cout << "Botão 0 pressionado, saindo..." << std::endl;
                            return false;
                        }
                        break;
                    default:
                        break;
                }
            }
        }

        return true;
    }

private:
    int fd = -1;               // File descriptor para o dispositivo
    char name[128] = "Desconhecido";  // Nome do joystick
};

