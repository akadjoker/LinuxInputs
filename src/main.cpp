

int main() {
    Joystick joystick("/dev/input/js0");

    if (!joystick.isValid()) {
        return -1;
    }

    while (true) {
        // Lê eventos de joystick usando select()
        if (!joystick.readEvent()) {
            break;  // Sai do loop se o botão 0 for pressionado
        }
    }

    return 0;
}
