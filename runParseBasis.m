function [w,LOG] = runParse(fname,it)

tr_data = readVectors(strcat(fname,'.train'));
dev_data = readVectors(strcat(fname,'.dev'));
te_data = readVectors(strcat(fname,'.test'));
[w,LOG] = trainBasis(tr_data,dev_data,it);
testerr = testBasis(w,te_data);
fprintf('test err :%5.3f \n',testerr);
