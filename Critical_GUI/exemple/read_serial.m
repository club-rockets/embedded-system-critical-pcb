function [data] = read_serial(data_mux1, data_mux2, N_values, comport)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% comport est 'COM1' et etc
% data_mux est 'a' ou 'b' et est ce qu'on veut recevoir
% N_values est le nombre de sample a receuillir
% Ts est le sampling time, il modifie celui du DSP...
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%declaration de la variable de retour
clc;
y = zeros(1,N_values*2);

% Ouverture du port de communication
s = serial(comport);
s.DataTerminalReady= 'off';

set(s,'InputBufferSize',40960);
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
fwrite(s, data_mux1, 'uint8');
fwrite(s, data_mux2, 'uint8');
y = fread(s, N_values*2,'single');




% apres la lecture on ferme la communication
fwrite(s, ')', 'uint8');% clos la communication
fwrite(s, ')', 'uint8');
    
data(1,:) = linspace(0,N_values,N_values);
data(1,:) = data(1,:).*0.000045620437956204379562043795620438;

for i=1:N_values
    data(3,i) = y(2*i-1);   
    data(2,i) = y(2*i); 
end

    
    

flushinput(s);
fclose(s);              % ferme le port serie
delete(s)

end