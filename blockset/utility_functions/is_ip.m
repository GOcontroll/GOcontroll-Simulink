function flag = is_ip(ipaddress)
% IS_IP function validates whether an ip address is valid or not
%
% bool = IS_IP(ipaddress)

Valid_lengths = (7:15);
num_array = ones(1,4);
Check_flag = 1;
%==== Error Checking =======%
if ~ischar(ipaddress)
  flag = false;
  return;
else
  length_ip = length(ipaddress);
  if ~ismember(length_ip,Valid_lengths)
    flag = false;
    return;  
  else
    ipaddress = strcat(ipaddress,'.');  
    char_count = 1;
    part_count = 0;
    cr = [];
    part_array = cell(1,4);
    for index=1:4
        
     while (strcmpi(cr,'.') || char_count <=(part_count+3)) && num_array(index)==1
     cr = ipaddress(char_count);
     if ~strcmpi(ipaddress(char_count),'.')
     part_array{index} = strcat(part_array{index},ipaddress(char_count));
     else
     num_array(index) = 0;    
     end
     char_count = char_count + 1;
     end
     
     if isempty(part_array{index})
         part_array{index} = '999';
     end    
     
     if num_array(index) == 1 && strcmpi(ipaddress(char_count),'.') && ~isempty(str2num(part_array{index})) 
         
     temp_num = str2num(part_array{index});
     if ~(temp_num>=0 && temp_num<=255) 
     Check_flag = 0;
     num_array = zeros(1,4);
     break;    
     end 
     char_count = char_count + 1;
     num_array(index) = 0;
     
     elseif num_array(index) == 0 && ~isempty(str2num(part_array{index}))
     
     temp_num = str2num(part_array{index});
     if ~(temp_num>=0 && temp_num<=255) 
     Check_flag = 0;
     num_array = zeros(1,4);
     break;    
     end
     num_array(index) = 0;
     
     else
     Check_flag = 0;
     num_array = zeros(1,4);
     break;
     
     end
     
     part_count = char_count;
    end
  end
  
 if Check_flag == 0 && (sum(num_array) == 0)
  flag = false;   
 else 
  flag = true;   
 end
end