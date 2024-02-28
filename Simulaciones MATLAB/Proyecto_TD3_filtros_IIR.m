close all
clear
%Funcion de transferencia en z
%Dividimos el numerador por la frec de muestreo

%primer filtro
%Pasa Bajos fc=160 hz
%numz1 = [0.02254];
%denz1 = [1  -0.9775];
%zplane([0.02254], [1  -0.9775]);
%fvtool(numz1,denz1);
%sFiltrada(n)=0.02254*sMuestreada(n)+0.9775*sFiltrada(n-1);
%sFiltrada2(n)=0.02254*sFiltrada(n)+0.9775*sFiltrada2(n-1);
% **********************************************************************+

%segundo filtro
%Pasa Banda fc1=160 hz fc2= 300 hz
%numz21=[0.987 -0.987];
%denz21=[1 -0.9775];
%numz22=[0.02092 0.02092];
%denz22=[1 -0.9582];
%zplane(numz21, denz21);
%fvtool(numz21,denz21);
%zplane(numz22, denz22);
%fvtool(numz22,denz22);
%sFiltrada(n)=0.5*sMuestreada(n)-0.5*sMuestreada(n-1)+0.9775*sFiltrada(n-1);
%sFiltrada2(n)=0.1*sFiltrada(n)+0.1*sFiltrada(n-1)+0.9582*sFiltrada2(n-1);

%roots(denz2)

%tercer filtro
%Pasa Banda fc1=300 hz fc2= 600 hz
%numz31=[0.9791 -0.9791];
%denz31=[1 -0.9582];
%numz32=[0.04099 0.04099];
%denz32=[1 -0.918];
%zplane(numz31, denz31);
%fvtool(numz31,denz31);
%zplane(numz32, denz32);
%fvtool(numz32,denz32);
%sFiltrada(n)=0.5*sMuestreada(n)-0.5*sMuestreada(n-1)+0.9582*sFiltrada(n-1);
%sFiltrada2(n)=0.25*sFiltrada(n)+0.25*sFiltrada(n-1)+0.918*sFiltrada2(n-1);
%roots(denz2)

%cuarto filtro
%Pasa Banda fc1=600 hz fc2= 1200 hz
%numz41=[0.959 -0.959];
%denz41=[1 -0.918];
%numz42=[0.01181 0.01181];
%denz42=[1 -0.8425];
%zplane(numz41, denz41);
%fvtool(numz41,denz41);
%zplane(numz42, denz42);
%fvtool(numz42,denz42);
%sFiltrada(n)=1*sMuestreada(n)-1*sMuestreada(n-1)+0.918*sFiltrada(n-1);
%sFiltrada2(n)=0.3*sFiltrada(n)+0.3*sFiltrada(n-1)+0.8425*sFiltrada2(n-1);
%roots(denz2)

%quinto filtro
%Pasa Banda fc1=1200 hz fc2= 2400 hz
%numz51=[0.09212 -0.09212];
%denz51=[1 -0.8425];
%numz52=[0.0219 0.0219];
%denz52=[1 -0.708];
%zplane(numz51, denz51);
%fvtool(numz51,denz51);
%zplane(numz52, denz52);
%fvtool(numz52,denz52);
%sFiltrada(n)=0.5*sMuestreada(n)-0.5*sMuestreada(n-1)+0.8425*sFiltrada(n-1);
%sFiltrada2(n)=0.6*sFiltrada(n)+0.6*sFiltrada(n-1)+0.708*sFiltrada2(n-1);
%sFiltrada3(n)=0.25*sFiltrada2(n)+0.25*sFiltrada2(n-1)+0.708*sFiltrada3(n-1);
%roots(denz2)

%sexto filtro
%Pasa Banda fc1=2400 hz fc2= 5000 hz
%numz61=[0.854 -0.854];
%denz61=[1 -0.708];
%numz62=[0.03939 0.03939];
%denz62=[1 -0.4747];
%zplane(numz61, denz61);
%fvtool(numz61,denz61);
%zplane(numz62, denz62);
%fvtool(numz62,denz62);
%sFiltrada(n)=1*sMuestreada(n)-1*sMuestreada(n-1)+0.708*sFiltrada(n-1);
%sFiltrada2(n)=0.5*sFiltrada(n)+0.5*sFiltrada(n-1)+0.4747*sFiltrada2(n-1);
%sFiltrada3(n)=0.5*sFiltrada2(n)+0.5*sFiltrada2(n-1)+0.4747*sFiltrada3(n-1);
%sFiltrada4(n)=1*sFiltrada3(n)-1*sFiltrada3(n-1)+0.708*sFiltrada4(n-1);
%roots(denz2)

%septimo filtro
%Pasa Banda fc1=5000 hz fc2= 10000 hz
%numz71=[0.7373 -0.7373];
%denz71=[1 -0.4747];
%numz72=[0.0624 0.0624];
%denz72=[1 -0.168];
%zplane(numz71, denz71);
%fvtool(numz71,denz71);
%zplane(numz72, denz72);
%fvtool(numz72,denz72);
%sFiltrada(n)=1*sMuestreada(n)-1*sMuestreada(n-1)+0.4747*sFiltrada(n-1);
%sFiltrada2(n)=1*sFiltrada(n)+1*sFiltrada(n-1)+0.168*sFiltrada2(n-1);
%sFiltrada3(n)=1*sFiltrada2(n)-1*sFiltrada2(n-1)+0.4747*sFiltrada3(n-1);
%sFiltrada4(n)=0.5*sFiltrada3(n)+0.5*sFiltrada3(n-1)+0.168*sFiltrada4(n-1);
%roots(denz2)

%octavo filtro
%Pasa altos fc= 10000 hz
%numz8=[0.584 -0.584];
%denz8=[1 -0.168];
%zplane(numz8, denz8);
%fvtool(numz8,denz8);
%sFiltrada(n)=1*sMuestreada(n)-1*sMuestreada(n-1)+0.168*sFiltrada(n-1);
%sFiltrada2(n)=0.75*sFiltrada(n)-0.75*sFiltrada(n-1)+0.168*sFiltrada2(n-1);
%roots(denz2)


%Para probar luego
 fm= 44100;
 t = [0:1/fm:0.5];
%Generamos una señal muestreada
%sMuestreada =2+ sin(2*pi*100*t) +sin(2*pi*250*t)+sin(2*pi*500*t) +sin(2*pi*750*t) +  sin(2*pi*1000*t) + sin(2*pi*2000*t) + sin(2*pi*3500*t) +sin(2*pi*5000*t) + sin(2*pi*10000*t)+sin(2*pi*20000);
sMuestreada = 2 + sin(2*pi*15000*t);
subplot(4,1,1), plot(t,sMuestreada);
title('Señal muestreada');
% 
% %Espectro de la señal muestreada
f=linspace(-fm/2, fm/2, length(sMuestreada));
espectro_sMuestreada=fftshift(abs(fft(sMuestreada)));
subplot(4,1,2),stem(f,espectro_sMuestreada);
xlim([0 fm/2]);
title('Espectro de señal muestreada');
% 
%Señal filtrada
sFiltrada(1) =0;
sFiltrada(2)=0;
sFiltrada2(1) =0;
sFiltrada2(2)=0;
sFiltrada3(1) =0;
sFiltrada3(2)=0;
sFiltrada4(1) =0;
sFiltrada4(2)=0;
for n=3:1:length(sMuestreada)
    sFiltrada(n)=1*sMuestreada(n)-1*sMuestreada(n-1)+0.168*sFiltrada(n-1);
    sFiltrada2(n)=0.75*sFiltrada(n)-0.75*sFiltrada(n-1)+0.168*sFiltrada2(n-1);
end
subplot(4,1,3),plot(t,sFiltrada2);
title('Señal filtrada');
% 
% %Espectro de señal filtrada
espectro_sFiltrada = fftshift(abs(fft(sFiltrada2)));
subplot(4,1,4),stem(f,espectro_sFiltrada);
title('Espectro señal filtrada');
xlim([0 fm/2]);