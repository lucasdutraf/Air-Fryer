#include "main.h"
#include "ui.h"

void finish();
void main_loop();
void menu();
void define_reference();

float TA = 0, TI = 0, TE = 0, control_output = 0;
int reference_potentiometer = 1;

int main(int argc, char *argv[])
{
    pthread_t tid[2];

    visor_setup();
    gpio_setup();
    bme280_setup();
    csv_setup();
    init_uart();

    pthread_create(&tid[0], NULL, (void *)main_loop, (void *)NULL);
    pthread_create(&tid[1], NULL, (void *)dashboard, (void *)NULL);

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);

    return (0);
}

void finish()
{
    deactivate_fan_and_resistor();
    ClrLcd();
    close_bme280();
    close_uart();
    refresh();
    endwin();
    exit(0);
}

void main_loop()
{
    float Kp = 5.0, Ki = 1.0, Kd = 5.0;
    float aux_TA;
    int time_to_write_csv = 0;

    signal(SIGINT, finish);
    pid_configure_constants(Kp, Ki, Kd);

    while (1)
    {

        if (reference_potentiometer)
        {
            aux_TA = reference_temperature(TA);
            if (aux_TA > TE && aux_TA < 100)
            {
                TA = aux_TA;
            }
        }

        TI = internal_temperature(TI);
        TE = bme280_temperature();

        print_lcd(TA, TI, TE);

        pid_update_reference(TA);

        control_output = pid_control(TI);

        manage_gpio_devices(control_output);

        if (time_to_write_csv == 0)
            time_to_write_csv++;
        else if (time_to_write_csv == 1)
        {
            write_csv(TA, TE, TI, control_output);
            time_to_write_csv = 0;
        }

        show_infos(TA, TI, TE, control_output, reference_potentiometer);

        usleep(700000);
    }
}

void menu()
{
    int choose;
    do
    {
        printf("-----------------------------------------\n");
        printf("Temperatura Referência: %4.2f\n", 12.8);
        printf("Temperatura Interna: %4.2f\n", 12.8);
        printf("Temperatura Externa: %4.2f\n", 12.8);
        printf("-----------------------------------------\n");
        printf("Resistor: \n");
        printf("Ventoinha: \n");
        printf("-----------------------------------------\n");
        printf("0. Usar temperatura do potenciômetro como referência\n");
        printf("1. Definir uma temperatura referência\n");
        scanf("%d", &choose);
        if (choose == 1)
        {
            define_reference();
        }
        else if (choose == 0)
        {
            reference_potentiometer = 1;
        }
    } while (1);
}

void define_reference()
{
    float reference;
    printf("Temperatura referência: ");
    scanf("%f", &reference);
    if (reference <= TE || reference >= 100)
    {
        reference_potentiometer = 0;
    }
}

void set_reference_input(int use_potenciometer, float new_reference)
{
    printf("%4.2f", new_reference);
    reference_potentiometer = use_potenciometer;
    if (!use_potenciometer && new_reference > TE && new_reference < 100)
    {
        pid_update_reference(new_reference);
        TA = new_reference;
    }
}
