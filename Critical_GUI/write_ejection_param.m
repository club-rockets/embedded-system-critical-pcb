function write_ejection_param(comport, ultrasonic_delay, apogee_ejection_delay, main_ejection_altitude)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% comport est 'COM1' (ecris comme ca!!!!)
% 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
clc;
delete(instrfindall);

% Ouverture du port de communication
% speed doesnt really matter, virtual comm port
s = serial(comport);
s.DataTerminalReady= 'off';
set(s,'InputBufferSize',100);
set(s,'BaudRate',115200);
set(s,'Parity','none');
set(s,'StopBits',1);
set(s,'DataBits',8);
set(s,'Terminator','');
set(s, 'Timeout', 50);
fopen(s);

%write ultrasonic delay
string = num2str(ultrasonic_delay);
string = strcat('g',string);
fwrite(s, string, 'uint8');
pause(0.1);

%write apogee ejection delay
string = num2str(apogee_ejection_delay);
string = strcat('h',string);
fwrite(s, string, 'uint8');
pause(0.1);

%write main ejection ejection
string = num2str(main_ejection_altitude);
string = strcat('i',string);
fwrite(s, string, 'uint8');
pause(0.1);

flushinput(s);        % flush le buffer
fclose(s);              % ferme le port serie
delete(s);              % delete le port serie
end