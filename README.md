# Estaci-n-de-Audio-R-tmica
Este repositorio incluye los programas utilizados para implementar una estación de audio rítmica: vúmetro digital, juego de luces y control de servomotores. Se ha realizado para la materia Técnicas Digitales 3 de la UNLAM

Se incluyen los siguientes archivos:
-Neopixel_3: Programa en C que contiene el código para controlar las matrices LED GRB 8x8 en cascada, permitiendo realizar un vúmetro, un arcoiris y pudiendo ser escalable para futuros proyectos
-Servo_tower_2: Programa en C que se utilizó para las pruebas del control de servomotores mediante PWM y accionar juego de luces en función de señales digitales
-IIR_1: Programa en C donde se realizaron las distintas pruebas para implementar filtros digitales, incluye tanto filtros IIR como FIR comentados para medias móviles. Se destaca que se agregó parte del código de Neopixel_3 para probar que el microcontrolador STM32F411XET6 pueda funcionar realizando todas las tareas a la vez, puesto que la velocidad de procesamiento que demandan tanto los filtros digitales como el control de las matrices LED es bastante alta.
-Proyecto_TD3_MC.ipybn: Programa en Python donde se probó la influencia de la tolerancia de los componentes pasivos en la respuesta en frecuencia del filtro anti alias mediante método de Monte Carlo
