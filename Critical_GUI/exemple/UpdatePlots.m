function UpdatePlots(hLine,y)
set(hLine(1), 'YData', y)
% set(hLine(2), 'YData', pb) 
% set(hLine(3), 'YData', mag)    
% set(hLine(4), 'YData', sub_esti)
% title(hAx(1), num2str(i,'Interval = %d'))
drawnow
end