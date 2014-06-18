function err = test(w,te_data)

S = length(te_data);
narcs=0;
nwrong = 0;

v0 = te_data{1}.v;
vlength = (length(v0)*2)^2;
pred_arcs = zeros(10000,vlength);
for s = 1:S,
    arcs = te_data{s};
    T = length(arcs);

    narcs = narcs + T -1;
    %pred_arcs = zeros(T*T,vlength);
    j = 1;
    for fr = 1:T,
        for to = 1:T,
            %    prec_arcs = predict([arcs(to).v arcs(fr).v ]) % implement your favorite ML algorithm to predict arc score
            pred_arcs(j,:) = outerproduct([arcs(to).v arcs(fr).v],[arcs(to).v arcs(fr).v]);
            j = j+1;
        end
    end
    score = pred_arcs(1:T*T,:)*w;
%    score = pred_arcs*w;
    score = reshape(score(:),T,T);
    score(logical(eye(T))) = -99999999999;
    score(1,:) = -9999999999;
    
    hguessed = decoder(score,T)'+1;
    hgold = [arcs(:).h];
    
    for tok = 2:T;
        if hguessed(tok) ~= hgold(tok), 
            nwrong = nwrong + 1;
        end
    end
end
err = nwrong/narcs;
end
