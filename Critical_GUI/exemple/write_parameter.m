function write_param(data_mux3, value, comport)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% comport est 'COM1' et etc
% data_mux est 'a' ou 'b' et est ce qu'on veut recevoir
% N_values est le nombre de sample a receuillir
% Ts est le sampling time, il modifie celui du DSP...
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%declaration de la variable de retour
clc;

% Ouverture du port de communication
s = serial(comport);
s.DataTerminalReady= 'off';

set(s,'InputBufferSize',50);
set(s,'BaudRate',921600);
set(s,'Parity','none');
set(s,'StopBits',1);
set(s,'DataBits',8);
set(s,'Terminator','');
set(s, 'Timeout', 100);
%set(s, 'ByteOrder','bigEndian');
fopen(s);


fwrite(s, '(', 'uint8');
fwrite(s, '(', 'uint8');
fwrite(s, '?', 'uint8');
fwrite(s, '?', 'uint8');
fwrite(s, data_mux3, 'uint8');

single(value)
fwrite(s, single(value), 'single');

% apres la lecture on ferme la communication
fwrite(s, ')', 'uint8');
fwrite(s, ')', 'uint8');

flushinput(s);
fclose(s);              % ferme le port serie
delete(s);

end