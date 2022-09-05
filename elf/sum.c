unsigned long long bias;
unsigned long long sum(unsigned long long *a,unsigned long long n){
    unsigned long long res;
    for(unsigned int i=0; i<n; i++){
        res += a[i];
    }
    return res+bias;
}
