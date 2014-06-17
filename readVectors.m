function data = readVectors(inputFile)
fid = fopen(inputFile);
tline = fgetl(fid);

data = {};
arc.h = -1;
arc.v = -1;
s = [arc];
j = 1;
while ischar(tline)
	r = sscanf(tline, '%f');
    r = r';
    if r(1) == -2,
        data{j} = s(2:end);
        s = [];
        j = j+1;
    end
    arc.h = r(1);
    arc.v = r(2:end);
    s = [s;arc];
	tline = fgetl(fid);
end
fclose(fid);
end