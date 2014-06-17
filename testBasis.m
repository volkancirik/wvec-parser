function err = test(w,te_data)

S = length(te_data);
narcs=0;
nwrong = 0;

for s = 1:S,
    arcs = te_data{s};
    T = length(arcs);

    narcs = narcs + T -1;
    pred_arcs = [];
    for fr = 1:T,
        for to = 1:T,
            %    prec_arcs = predict([arcs(to).v arcs(fr).v ]) % implement your favorite ML algorithm to predict arc score
            pred_arcs = [pred_arcs;kron([arcs(to).v 1],[arcs(fr).v 1])];
        end
    end
    score = pred_arcs*w;
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
