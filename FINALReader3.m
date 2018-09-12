close all;
clear all;
clc;
clear();

%fid = fopen('Pro3Data1.txt');%q1
%fid = fopen('Pro3Data2.txt');%q2
fid = fopen('Pro3Data3B.txt');%q3
%fid = fopen('Pro3Data3.txt');%q3 BAD
%res = fscanf(fid, '%i\t%i', [2,265]); %q1
%res = fscanf(fid, '%i\t%i', [2,195]); %q2
res = fscanf(fid, '%i\t%i', [2,199]);  %q3B
%res = fscanf(fid, '%i\t%i', [2,230]); %q3 BAD


res = res';
%s = 0:1:264;    %q1
%s = 0:1:194;    %q2
s = 0:1:198;     %q3B
%s = 0:1:229;    %q3 BAD

s = s';
s = s*2;        %q1+2
sense = res(:,1);
pump = res(:,2);
%pump = pump*255;%q1
%pot = pot';
%led = led';
plot(s, sense, 'r-');
hold on;
plot(s, pump, 'b-');
xlabel('Time [s]');
ylabel('Sensor Value (Red) Pump (Blue)');
title('Arduino');
fclose(fid);