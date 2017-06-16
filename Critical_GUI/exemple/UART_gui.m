function [] = UART_gui()
data_mux1 = 'a';
data_mux2 = 'a';
data_mux3 = 'a';
do_it_first = 1;
clc;

%  Create and then hide the GUI as it is being constructed.
S.fh = figure('Visible','off','position',[600 600 420 300],'name','Console UART - Dual Channel');

% Construct the components.
S.resetcom_pb    = uicontrol('Style','pushbutton',...
                 'String','del(instrfindall)','Position',[300,230,100,40]);

S.Write_Val_pb   = uicontrol('Style','pushbutton',...
                 'String','Write Value','Position',[300,165,100,40]);         
         
S.scope_pb       = uicontrol('Style','pushbutton',...
                 'String','Start Scope','Position',[300,100,100,40]);

S.tx_1           = uicontrol('Style','text','String','Channel 2',...
                 'Position',[20,45,60,15]);
      
S.popup_data_mux2 = uicontrol('Style','popupmenu','String',{...
            'Alim_1V8',...
            'Alim_3V3',...
            'Alim_5V',...
            'CAP1_voltage',...
            'CAP2_voltage',...
            'DC_current',...
            'DC_voltage',...
            'DC_power',...
            'Grid_Current',...
            'Grid_Voltage'...
            'BRIDGE_consigne'...
            'BRIDGE_PID_error'...
            'PLL_output_grid'...
            'PUC_consigne'...
            'PUC_Current'...
            'PUC_PID_error'...
            'PUC_CAP_charge'...
            },'Position',[90,50,150,15]);

S.tx_2  = uicontrol('Style','text','String','Channel 1',...
            'Position',[20,85,60,15]);
         
S.popup_data_mux1 = uicontrol('Style','popupmenu','String',{...
            'Alim_1V8',...
            'Alim_3V3',...
            'Alim_5V',...
            'CAP1_voltage',...
            'CAP2_voltage',...
            'DC_current',...
            'DC_voltage',...
            'DC_power',...
            'Grid_Current',...
            'Grid_Voltage'...
            'BRIDGE_consigne'...
            'BRIDGE_PID_error'...
            'PLL_output_grid'...
            'PUC_consigne'...
            'PUC_Current'...
            'PUC_PID_error'...
            'PUC_CAP_charge'...
            },'Position',[90,90,150,15]);        
     
S.tx_3           = uicontrol('Style','text','String','Parameter',...
                 'Position',[20,125,60,15]);
      
S.popup_data_mux3 = uicontrol('Style','popupmenu','String',{...
            'BRIDGE_OUTPUT_VREF',...
            'PUC_OUTPUT_CURRENT_REF',...
            'BRIDGE_DUTY_CYCLE_REF',...
            },'Position',[90,130,150,15]);    

S.popup_param_val = uicontrol('Style','text',...
             'String','Enter parameter value:','Position',[20,170,140,20],...
             'Callback',{@applybutton_Callback,S});
         
S.param_val = uicontrol('style','edit',...
                 'unit','pix',...
                 'position',[170 170 70 20],...
                 'fontsize',10,...
                 'string','50');
             
S.comm_tx    = uicontrol('Style','text',...
             'String','Enter Comm Port used:','Position',[20,250,140,20],...
             'Callback',{@applybutton_Callback,S});
         
S.ed = uicontrol('style','edit',...
                 'unit','pix',...
                 'position',[170 250 70 20],...
                 'fontsize',10,...
                 'string','4');
             
S.comm_N    = uicontrol('Style','text',...
             'String','Enter number of sample:','Position',[20,210,140,20],...
             'Callback',{@applybutton_Callback,S});
         
S.N_values = uicontrol('style','edit',...
                 'unit','pix',...
                 'position',[170 210 70 20],...
                 'fontsize',10,...
                 'string','50');
             
S.repeat_scope = uicontrol('style','checkbox',...
             'String','repeat scope','position',[300,40,100,30]);

S.mem = [0,0,0];




% Set the callbacks for pushbuttons.
set(S.resetcom_pb,'callback',{@resetcom_Callback,S});  
set(S.scope_pb,'callback',{@scope_Callback,S});
set(S.Write_Val_pb,'callback',{@write_param_Callback,S});

% align([happly,hmesh,hcontour,htext1,hpopup1],'Center','None');
align([S.tx_1,S.popup_data_mux1],'None','Center');

% Assign the GUI a name to appear in the window title.
set(S.fh,'Name','UART_gui');
% 
% Move the GUI to the center of the screen.
movegui(S.fh,'center');

% Make the GUI visible.
set(S.fh,'Visible','on');


get(S.repeat_scope,'value');

function [] = resetcom_Callback(varargin)
    delete(instrfindall);
end



function [] = write_param_Callback(varargin)
    comport = get(S.ed,'string');
    comport = sprintf('COM%c',comport);    
    param_val = str2num(get(S.param_val, 'string'));
    
    switch get(S.popup_data_mux3,'value')
        case 1 %alim 1V8
            data_mux3 = 'a';
        case 2 %
            data_mux3 = 'b';
        case 3 %
            data_mux3 = 'c';
    end  
    
    write_param(data_mux3, param_val, comport);
    
    
end

 
function [] = scope_Callback(varargin)
    
    while(get(S.repeat_scope,'value') || do_it_first)
        do_it_first = 0;
S = varargin{3};  % Get the structure.

    comport = get(S.ed,'string');
    comport = sprintf('COM%c',comport);
    N_values = str2num(get(S.N_values, 'string'));
    
        switch get(S.popup_data_mux1,'value')
            case 1 %alim 1V8
                data_mux1 = 'a';
            case 2 %
                data_mux1 = 'b';
            case 3 %
                data_mux1 = 'c';
            case 4 %
                data_mux1 = 'd';
            case 5 %
                data_mux1 = 'e';
            case 6 %
                data_mux1 = 'f';
            case 7 %
                data_mux1 = 'g';
            case 8 %
                data_mux1 = 'h';
            case 9 %
                data_mux1 = 'i';
            case 10 %
                data_mux1 = 'j';
            case 11
                data_mux1 = 'k';
            case 12
                data_mux1 = 'l';
            case 13
                data_mux1 = 'm';
            case 14
                data_mux1 = 'n';
            case 15
                data_mux1 = 'o';
            case 16
                data_mux1 = 'p';
            case 17
                data_mux1 = 'q';              
        end
        
        
        switch get(S.popup_data_mux2,'value')
            case 1 %alim 1V8
                data_mux2 = 'a';
            case 2 %
                data_mux2 = 'b';
            case 3 %
                data_mux2 = 'c';
            case 4 %
                data_mux2 = 'd';
            case 5 %
                data_mux2 = 'e';
            case 6 %
                data_mux2 = 'f';
            case 7 %
                data_mux2 = 'g';
            case 8 %
                data_mux2 = 'h';
            case 9 %
                data_mux2 = 'i';
            case 10 %
                data_mux2 = 'j';
            case 11
                data_mux2 = 'k';
            case 12
                data_mux2 = 'l';
            case 13
                data_mux2 = 'm';
            case 14
                data_mux2 = 'n';
            case 15
                data_mux2 = 'o';
            case 16
                data_mux2 = 'p';
            case 17
                data_mux2 = 'q';  
        end
             
        
    samples_graph = read_serial(data_mux1, data_mux2,N_values, comport);
    

    figure(10);
    
    subplot(211);
    plot(samples_graph(1,:),samples_graph(2,:));
    %ylim([-10 10]);
    title('Channel 1');
    grid on;
    
    subplot(212);
    plot(samples_graph(1,:),samples_graph(3,:));
    %ylim([-10 10]);
    title('Channel 2');
    grid on;    
    
%     if(data_mux1 == 'm' || data_mux2 == 'm')
%         figure(11);
%         grid on;
%         plot(samples_graph(1,:),samples_graph(2,:),'Color','red');
%         hold on;
%         plot(samples_graph(1,:),samples_graph(3,:),'Color','blue');
%         hold off;
%     end

    end
    do_it_first = 1;
end
    
    
end
