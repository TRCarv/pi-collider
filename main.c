#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h> 

#define WIDTH 900
#define HEIGHT 600
#define BIG 80
#define SMALL 50


typedef struct {
    double x, vx, m;
} Block;


Block big;
Block small;
int time_steps;


void reset_simulation(int *count, int pi_digits) {
    *count = 0;
    
    // Calcula a massa
    double mass_big = pow(100, pi_digits - 1);
    
    big = (Block){0.5 * WIDTH, -200, mass_big};
    small = (Block){0.25 * WIDTH, 0, 1};
    
    // Ajusta os steps dinamicamente
    time_steps = 100 * (int)pow(10, pi_digits - 1);
    if (time_steps < 10000) time_steps = 10000; 
}

void draw_boxs(){
    DrawRectangle(big.x, HEIGHT*0.55, BIG, BIG, RAYWHITE);
    DrawRectangle(small.x, HEIGHT*0.55 + BIG - SMALL, SMALL, SMALL, RAYWHITE);
}

void draw_walls(){
    DrawRectangle(0, 0, 2, HEIGHT*0.55 + BIG, RAYWHITE);
    DrawRectangle(0, HEIGHT*0.55 + BIG, WIDTH, 2, RAYWHITE);
}

void step_blocks(float dt){
    big.x += big.vx * dt;
    small.x += small.vx * dt;
}

void block_collision(){
    float init_vx_big = big.vx;
    float init_vx_small = small.vx;
    big.vx = ((big.m - small.m)/(big.m + small.m))*init_vx_big + (2*small.m/(big.m+small.m))*init_vx_small;
    small.vx = ((2*big.m)/(big.m+small.m))*init_vx_big + ((small.m - big.m)/(big.m +small.m))*init_vx_small;
}

void draw_counter(int count, int pi_digits){
    DrawText(TextFormat("Colisões: %d", count), 50, HEIGHT*0.1, 30, RED);
    DrawText(TextFormat("Massa Bloco Maior: %.0f kg", big.m), 50, HEIGHT*0.20, 20, LIGHTGRAY);
    DrawText("Massa do Bloco Menor: 1 kg", 50, HEIGHT*0.25, 20, LIGHTGRAY);
}

int main() {
    int count = 0;
    int pi_digits = 3; 
    bool editMode = false; 
    
    InitWindow(WIDTH, HEIGHT, "Determinando Pi por Colisões Elásticas");
    SetTargetFPS(60);

    // Carrega o estado inicial
    reset_simulation(&count, pi_digits);

    while (!WindowShouldClose()) {
        // ==== Física ====
        float dt = GetFrameTime() / time_steps;
        
        for (int i = 0; i < time_steps; i++){
            step_blocks(dt);
            
            // Colisão com a parede 
            if (small.x <= 0 && small.vx < 0) {
                small.vx = -small.vx;
                count++;
            }
            
            // Colisão entre blocos 
            float dist = big.x - small.x - SMALL;
            if (dist <= 0 && big.vx < small.vx) {
                count++;
                block_collision();
            }
        }

        // ==== Desenho e Interface ====
        BeginDrawing();
        ClearBackground(BLACK);
        
        draw_counter(count, pi_digits);
        draw_boxs();
        draw_walls();        
        
        // --- RAYGUI ---
        
        // Botão de Reiniciar
        if (GuiButton((Rectangle){ 50, HEIGHT - 70, 120, 40 }, "Reiniciar")) {
            reset_simulation(&count, pi_digits);
        }

        // Controle numérico de Dígitos de Pi (com caixa de texto embutida)
        DrawText("Dígitos do Pi:", 190, HEIGHT - 60, 20, RAYWHITE);
        if (GuiSpinner((Rectangle){ 340, HEIGHT - 70, 120, 40 }, "", &pi_digits, 1, 6, editMode)) {
            editMode = !editMode; 
            reset_simulation(&count, pi_digits); // Reseta a simulação quando muda o valor
        }

        EndDrawing();
    }
    CloseWindow();
    return 0;
}