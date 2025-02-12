// Definição das biliotecas a serem utilizadas no programa
#include <stdio.h>
#include "pico/stdlib.h" 
#include "pico/time.h" 
#include "hardware/pwm.h" 

// Definição da GPIO 22 como pino PWM
#define ServPin 22

// Para que o período do PWM seja de 20ms e tomando um divisor de clock arbitrário igual a 40, tem-se um wrap de 62499
const float CLKDIV = 40.0;
const uint16_t WRAP = 62499;

// Os ciclos ativos de 2400us, 1470us e 500us foram calculados proporcionalmente ao wrap de 62499 e o período PWM de 20ms
const int ciclo1 = 7500;    //Ciclo de 2400us
const int ciclo2 = 4593;    //Ciclo de 1470us
const int ciclo3 = 1562;    //Ciclo de 500us

// Definição de variáveis que serão utilizadas como parâmetros ou flags ao longo do programa
static int nivel = 0;
static bool initial = true;
static bool rise = false;

// Função que controla o movimento periódico da flange do servomotor
void ciclo(){
    // Se a flag estiver em true, a flange se movimenta do ponto em que está gradualmente, com ciclos ativos cada vez maiores
    // O aumento é de 5us a cada incremento, o que corresponde a cerca de 16 de aumento na variável relacionada ao wrap (obtido por meio de cálculos)
    if(rise){
        nivel = nivel+16;
        // Se a variável passa de 7500 (cerca de 180º de giro), ela para de sofrer incremento 
        if(nivel>7500){
            // A variável é estabilizada em 7500 e a flag é colocada como false
            nivel = 7500;
            rise = false;
        }
    // Com a flag em false, a variável é decrementada até chegar a um valor menor que 1562
    } else {
        nivel = nivel-16;
        if(nivel<1562){
            // A variável é estabilizada em 1562 e a flag é colocada como true novamente
            nivel = 1562;
            rise = true;
        }
    }
    // A cada incremento ou decremento o pwm é modificado para o duty cycle atual marcado pela variável
    pwm_set_gpio_level(ServPin, nivel);
    // Delay para melhorar a visualização
    sleep_ms(10);
}

// Função para configurar o PWM e colocá
void pwm_setup(){

    gpio_set_function(ServPin, GPIO_FUNC_PWM); 
    uint sliceNum = pwm_gpio_to_slice_num(ServPin); 
    pwm_set_clkdiv(sliceNum, CLKDIV);
    pwm_set_wrap(sliceNum, WRAP);
    pwm_set_enabled(sliceNum, true);

    // O PWM é colocado em 0º inicialmente seguido de um delay breve para que seja possível ver sua movimentação inicial
    pwm_set_gpio_level(ServPin, ciclo3);
    sleep_ms(100);
}

int main(){
    stdio_init_all();
    // O PWM é configurado
    pwm_setup();

    // Loop infinito
    while(1){
        // Se a flag initial estiver ativa, o PWM tem ciclo ativo colocado em cada um dos valores de ciclo solicitados
        // Desse modo, ele fica na posição de 180º, 90º e 0º, respectivamente
        if(initial == true){
            pwm_set_gpio_level(ServPin, ciclo1);
            sleep_ms(5000);
            pwm_set_gpio_level(ServPin, ciclo2);
            sleep_ms(5000);
            pwm_set_gpio_level(ServPin, ciclo3);
            sleep_ms(5000);
            // A flag initial é desativada para evitar sua repetição e a flag rise é ativada para iniciar a rotina
            initial = false;
            rise = true;
        }
        // Com a flag initial não ativa, inicia-se a função ciclo
        else if(initial == false){
            ciclo();
        }
    }
}