% Ouverture du port de communication
s = serial('COM14');
set(s,'BaudRate',520833);
set(s,'StopBits',1);
set(s,'DataBits',8);
set(s,'Terminator','');
fopen(s);
 
for i = 1:500
     
    fwrite(s, 99, 'uint8');
    rx(i) = fread(s, 1,'uint32');

end
 
 plot(rx);
 grid on;
 
 
 fclose(s);