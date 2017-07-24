function update_time_date(comport)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% comport est 'COM1' (ecris comme ca!!!!
% 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
clc;

% get current time and date
c = clock;

% Ouverture du port de communication
% speed doesnt really matter, virtual comm port
% s = serial(comport);
% s.DataTerminalReady= 'off';
% set(s,'InputBufferSize',50);
% set(s,'BaudRate',921600);
% set(s,'Parity','none');
% set(s,'StopBits',1);
% set(s,'DataBits',8);
% set(s,'Terminator','');
% set(s, 'Timeout', 100);
% fopen(s);
delete(instrfind);
pause(0.1);
s = serial(comport);
set(s,'BaudRate',9600,'DataBits', 8, 'Parity', 'none','StopBits', 1, 'FlowControl', 'none','Terminator','CR/LF');
fopen(s);

%write year
string = num2str(c(1) - 2000);
string = strcat('a',string);
fwrite(s, string, 'uint8');
pause(0.1);

%write day
string = num2str(c(2));
string = strcat('b',string);
fwrite(s, string, 'uint8');
pause(0.1);

%write month
string = num2str(c(3));
string = strcat('c',string);
fwrite(s, string, 'uint8');
pause(0.1);

%write hour
string = num2str(c(4));
string = strcat('d',string);
fwrite(s, string, 'uint8');
pause(0.1);

%write minute
string = num2str(c(5));
string = strcat('e',string);
fwrite(s, string, 'uint8');
pause(0.1);

%write second
string = num2str(c(6));
string = strcat('f',string);
fwrite(s, string, 'uint8');
pause(0.1);

flushinput(s);        % flush le buffer
fclose(s);              % ferme le port serie
delete(s);              % delete le port serie
end