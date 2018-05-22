
%%
Fc = 400;
Fstop = 1000;
Fs = 8000;

Ntaps = 33;
order = Ntaps - 1;

Wnc = Fc/Fs*2;
Wns = Fstop/Fs*2;

Wn = Fstop/Fs*2;
% coeff = fir1(order,Wn);
coeff = firls(order, [0 Wnc Wns 1], [1 1 0 0]);

h=fvtool(coeff,'Fs',Fs);

%%

% Scale so that sum of absolute coefficients is smaller than 1
coeff = coeff ./ sum(abs(coeff));
% Scale to Q15 format
coeffQ15 = int16(floor(coeff .* 32767));
% Print ready to paste to header file
coeffQ15string = sprintf('%.0f,' , coeffQ15);
coeffQ15string = coeffQ15string(1:end-1);% strip final comma
outHeader = sprintf('//FIR-LP fs=%d fc=%d NTAPS=%d\n%s\n%s\n%s',Fs,Fstop,Ntaps,'fractional coefficients[NTAPS] __attribute__ ((space(xmemory),far)) = {',coeffQ15string,'};');
disp(outHeader)