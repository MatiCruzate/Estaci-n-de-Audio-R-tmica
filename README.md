# Estaci-n-de-Audio-R-tmica
Este repositorio incluye los programas utilizados para implementar una estación de audio rítmica: vúmetro digital, juego de luces y control de servomotores. 
Proyecto realizado para la carrera de Ingeniería en Electrónica de la UNLAM

Se incluyen los siguientes archivos:
-Audio_Ritmica: Prograa en C que contiene la aplicación principal, incluyendo todas las funciones utilizadas en las pruebas
-Neopixel_3: Programa en C que contiene el código para controlar las matrices LED GRB 8x8 en cascada, permitiendo realizar un vúmetro, un arcoiris y pudiendo ser escalable para futuros proyectos
-Servo_tower_2: Programa en C que se utilizó para las pruebas del control de servomotores mediante PWM y accionar juego de luces en función de señales digitales
-IIR_1: Programa en C donde se realizaron las distintas pruebas para implementar filtros digitales, incluye tanto filtros IIR como FIR comentados para medias móviles. Se destaca que se agregó parte del código de Neopixel_3 para probar que el microcontrolador STM32F411XET6 pueda funcionar realizando todas las tareas a la vez, puesto que la velocidad de procesamiento que demandan tanto los filtros digitales como el control de las matrices LED es bastante alta.
-Proyecto_TD3_MC.ipybn: Programa en Python donde se probó la influencia de la tolerancia de los componentes pasivos en la respuesta en frecuencia del filtro anti alias mediante método de Monte Carlo
-Modelo 3D: Modelo propio de contenedor para las matrices LED, servomotores y PCB del proyecto, se incluye el archivo extensión stl para impresión 3D.
Simulaciones MATLAB: Pruebas de los filtros digitales IIR en MATLAB
Informe-Cruzate_v3.pdf: El informe de la materia Técnicas Digitales 3 de la UNLAM acerca de este proyecto, donde está la explicación de cómo se realizó y se incluyen anexos para los cálculos y análisis complementarios
Presentación- Estación de Audio Rítmica: La presentación del proyecto realizada con CANVA
Camino_Critico_MC.ypbn: programa en Python donde se realizaron simulaciones computacionales basadas en el Método de Monte Carlo para corroborar el camino crítico en la evaluación de tareas a realizar en el proyecto.
