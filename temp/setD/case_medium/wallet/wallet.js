var h_balance = 100; /* high variable */
var l_obs = 0; /* low variable */

var max = 100, min = 100;
var debit_amount = Math.random() * (max - min) + min; /*0...100*/

/* MAIN JS PROGRAM */
console.log('init H_balance: ', h_balance)
console.log('(electronic wallet)')
while(h_balance >= debit_amount){
  h_balance = h_balance - debit_amount;
  l_obs = l_obs + 1;
}

console.log('debit_amount:   ', debit_amount)
console.log('final H_balance:', h_balance)
console.log('L_obs:          ', l_obs)

console.log('------------------------------------------')
console.log('forall A. exists B. exists C.')
console.log('G((L_obs[A] == L_obs[b]) /\\ (L_obs[A] == L_obs[C]) /\\ (H_balance[B] =/= H_balance[C]))')
console.log('------------------------------------------')

console.log('* max_balance=100, debit_amount=100, then L_obs[A]=1 is a counterexample')
/* END OF PROGRAM */
