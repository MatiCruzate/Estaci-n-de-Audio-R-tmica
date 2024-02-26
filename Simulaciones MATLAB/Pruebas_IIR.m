close all
clear

%calculos
%Hs=tf([4441700 0 0],[1 4086.2 8351900 7745100000 3590700000000]);
Hs1=tf([1 0],[1 31420])
Hz1=c2d(Hs1,1/44100,'tustin')
Hs2=tf(9425,[1 62830])
Hz2=c2d(Hs2,1/44100,'tustin')

%Funcion de transferencia en s
%num = [1 0];
%den = [1 6.283e4];
%bode(num,den);

%Funcion de transferencia en z
%numz = [100 -170.68];
%Dividimos el numerador por la frec de muestreo
%numz = [0.001107 -0.001141 -0.00104 0.001073];
%denz = [1 -3.907 5.726 -3.73 0.9115];
numz1=[0.7373 -0.7373];
denz1=[1 -0.4747];
numz2=[0.0624 0.0624];
denz2=[1 -0.168];
%zplane(0, 0.5);
fvtool(numz1,denz1);
fvtool(numz2,denz2);

fm= 44100;
t = [0:1/fm:0.5];
%Generamos una señal muestreada
sMuestreada = sin(2*pi*100)+sin(2*pi*200*t)+sin(2*pi*2000*t)+sin(2*pi*400*t) + sin(2*pi*5000*t) + sin(2*pi*7500*t) + sin(2*pi*10000*t) + sin(2*pi*20000*t);
%sMuestreada =sin(2*pi*1000*t) + sin(2*pi*5060*t) + sin(2*pi*20000*t);
subplot(4,1,1), plot(t,sMuestreada);
title('Señal muestreada');

%Espectro de la señal muestreada
f=linspace(-fm/2, fm/2, length(sMuestreada));
espectro_sMuestreada=fftshift(abs(fft(sMuestreada)));
subplot(4,1,2),stem(f,espectro_sMuestreada);
xlim([0 fm/2]);
title('Espectro de señal muestreada');

 %filtro 8
%Señal filtrada
%numz=[0.584 -0.584];
%denz=[1 -0.168];
%sFiltrada(1) =0;
%sFiltrada2(1) =0;
%for n=2:1:length(sMuestreada)
%     sFiltrada(n)=1*sMuestreada(n)-1*sMuestreada(n-1)+0.168*sFiltrada(n-1);
%end
%for n=2:1:length(sFiltrada)
%     sFiltrada2(n)=0.5*sFiltrada(n)-0.5*sFiltrada(n-1)+0.168*sFiltrada2(n-1);
%end
 
%subplot(4,1,3),plot(t,sFiltrada2);
%title('Señal filtrada');
 %Espectro de señal filtrada
% espectro_sFiltrada = fftshift(abs(fft(sFiltrada2)));
% subplot(4,1,4),stem(f,espectro_sFiltrada);
% title('Espectro señal filtrada');
%xlim([0 fm/2]); 



%filtro 7
%Señal filtrada
%numz1=[0.7373 -0.7373];
%denz1=[1 -0.4747];
%numz2=[0.0624 0.0624];
%denz2=[1 -0.168];
%sFiltrada(1) =0;
%sFiltrada2(1) =0;
%sFiltrada3(1)=0;
%sFiltrada4(1)=0;
%for n=2:1:length(sMuestreada)
%     sFiltrada(n)=1*sMuestreada(n)-1*sMuestreada(n-1)+0.4747*sFiltrada(n-1);
%end
%for n=2:1:length(sFiltrada)
%     sFiltrada2(n)=0.5*sFiltrada(n)+0.5*sFiltrada(n-1)+0.168*sFiltrada2(n-1);
%end 
% subplot(4,1,3),plot(t,sFiltrada2);
% for n=2:1:length(sFiltrada2)
%     sFiltrada3(n)=0.9*sFiltrada2(n)-0.9*sFiltrada2(n-1)+0.4747*sFiltrada3(n-1);
% end 
%for n=2:1:length(sFiltrada3)
%     sFiltrada4(n)=0.5*sFiltrada3(n)+0.5*sFiltrada3(n-1)+0.168*sFiltrada4(n-1);
%end
% subplot(4,1,3),plot(t,sFiltrada4);
% title('Señal filtrada');
%Espectro de señal filtrada
% espectro_sFiltrada = fftshift(abs(fft(sFiltrada4)));
% subplot(4,1,4),stem(f,espectro_sFiltrada);
%  title('Espectro señal filtrada');
%  xlim([0 fm/2]); 



%filtro 6
%Señal filtrada
%numz1=[0.854 -0.854];
%denz1=[1 -0.708];
%numz2=[0.03939 0.03939];
%denz2=[1 -0.4747];
sFiltrada(1) =0;
sFiltrada2(1) =0;
sFiltrada3(1)=0;
sFiltrada4(1)=0;
for n=2:1:length(sMuestreada)
     sFiltrada(n)=0.6*sMuestreada(n)-0.6*sMuestreada(n-1)+0.708*sFiltrada(n-1);
end
for n=2:1:length(sFiltrada)
     sFiltrada2(n)=0.5*sFiltrada(n)+0.5*sFiltrada(n-1)+0.4747*sFiltrada2(n-1);
end 
 for n=2:1:length(sFiltrada2)
     sFiltrada3(n)=0.4*sFiltrada2(n)+0.4*sFiltrada2(n-1)+0.4747*sFiltrada3(n-1);
 end 
for n=2:1:length(sFiltrada3)
     sFiltrada4(n)=1*sFiltrada3(n)-1*sFiltrada3(n-1)+0.708*sFiltrada4(n-1);
end
 subplot(4,1,3),plot(t,sFiltrada4);
 title('Señal filtrada');
%Espectro de señal filtrada
 espectro_sFiltrada = fftshift(abs(fft(sFiltrada4)));
 subplot(4,1,4),stem(f,espectro_sFiltrada);
  title('Espectro señal filtrada');
  xlim([0 fm/2]); 

%filtro 5
%Señal filtrada
%numz1=[0.09212 -0.09212];
%denz1=[1 -0.8425];
%numz2=[0.0219 0.0219];
%denz2=[1 -0.708];
%sFiltrada(1) =0;
%sFiltrada2(1) =0;
%sFiltrada3(1)=0;
%for n=2:1:length(sMuestreada)
%     sFiltrada(n)=0.3*sMuestreada(n)-0.3*sMuestreada(n-1)+0.8425*sFiltrada(n-1);
%end
%for n=2:1:length(sFiltrada)
%     sFiltrada2(n)=0.6*sFiltrada(n)+0.6*sFiltrada(n-1)+0.708*sFiltrada2(n-1);
%end 
% subplot(4,1,3),plot(t,sFiltrada2);
% for n=2:1:length(sFiltrada2)
%     sFiltrada3(n)=0.2*sFiltrada2(n)+0.2*sFiltrada2(n-1)+0.708*sFiltrada3(n-1);
%end 
% subplot(4,1,3),plot(t,sFiltrada3);
 
% title('Señal filtrada');
%Espectro de señal filtrada
% espectro_sFiltrada = fftshift(abs(fft(sFiltrada3)));
% subplot(4,1,4),stem(f,espectro_sFiltrada);
%  title('Espectro señal filtrada');
%  xlim([0 fm/2]);



% %filtro 1
 %Señal filtrada
 %sFiltrada(1) =0;
 %sFiltrada2(1) =0;
 %for n=2:1:length(sMuestreada)
 %     sFiltrada(n)=0.05*sMuestreada(n)+0.9775*sFiltrada(n-1);
 %end
 %for n=2:1:length(sFiltrada)
 %     sFiltrada2(n)=0.026*sFiltrada(n)+0.9775*sFiltrada2(n-1);
 % end
 % subplot(4,1,3),plot(t,sFiltrada2);
 % title('Señal filtrada');
 % 
 % %Espectro de señal filtrada
 % espectro_sFiltrada = fftshift(abs(fft(sFiltrada2)));
 % subplot(4,1,4),stem(f,espectro_sFiltrada);
 % title('Espectro señal filtrada');
 % xlim([0 fm/2]);
 
 %filtro 2
%Señal filtrada
%numz1=[0.987 -0.987];
%denz1=[1 -0.9775];
%numz2=[0.02092 0.02092];
%denz2=[1 -0.9582];
%sFiltrada(1) =0;
%sFiltrada2(1) =0;
%for n=2:1:length(sMuestreada)
%     sFiltrada(n)=0.32*sMuestreada(n)-0.32*sMuestreada(n-1)+0.9775*sFiltrada(n-1);
%end
%for n=2:1:length(sMuestreada)
%     sFiltrada2(n)=0.1*sFiltrada(n)+0.1*sFiltrada(n-1)+0.9582*sFiltrada2(n-1);
%end 
% subplot(4,1,3),plot(t,sFiltrada2);
 
% title('Señal filtrada');
%Espectro de señal filtrada
% espectro_sFiltrada = fftshift(abs(fft(sFiltrada2)));
% subplot(4,1,4),stem(f,espectro_sFiltrada);
%  title('Espectro señal filtrada');
%  xlim([0 fm/2]);
 
 %filtro 3
%Señal filtrada
%numz1=[0.9791 -0.9791];
%denz1=[1 -0.9582];
%numz2=[0.04099 0.04099];
%denz2=[1 -0.918];
%sFiltrada(1) =0;
%sFiltrada2(1) =0;
%for n=2:1:length(sMuestreada)
%     sFiltrada(n)=0.147*sMuestreada(n)-0.147*sMuestreada(n-1)+0.9582*sFiltrada(n-1);
%end
%for n=2:1:length(sMuestreada)
%     sFiltrada2(n)=0.41*sFiltrada(n)+0.41*sFiltrada(n-1)+0.918*sFiltrada2(n-1);
%end 
% subplot(4,1,3),plot(t,sFiltrada2);
 
% title('Señal filtrada');
%Espectro de señal filtrada
% espectro_sFiltrada = fftshift(abs(fft(sFiltrada2)));
% subplot(4,1,4),stem(f,espectro_sFiltrada);
%  title('Espectro señal filtrada');
%  xlim([0 fm/2]);
 
  %filtro 4
%Señal filtrada
%numz1=[0.959 -0.959];
%denz1=[1 -0.918];
%numz2=[0.01181 0.01181];
%denz2=[1 -0.8425];
%sFiltrada(1) =0;
%sFiltrada2(1) =0;
%for n=2:1:length(sMuestreada)
%     sFiltrada(n)=0.96*sMuestreada(n)-0.96*sMuestreada(n-1)+0.918*sFiltrada(n-1);
%end
%for n=2:1:length(sMuestreada)
%     sFiltrada2(n)=0.15*sFiltrada(n)+0.15*sFiltrada(n-1)+0.8425*sFiltrada2(n-1);
%end 
% subplot(4,1,3),plot(t,sFiltrada2);
 
% title('Señal filtrada');
%Espectro de señal filtrada
% espectro_sFiltrada = fftshift(abs(fft(sFiltrada2)));
% subplot(4,1,4),stem(f,espectro_sFiltrada);
%  title('Espectro señal filtrada');
%  xlim([0 fm/2]);