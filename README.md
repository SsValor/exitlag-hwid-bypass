# ExitLag HWID Bypass
<a href="https://www.exitlag.com/" target="_blank"><img src="https://www.exitlag.com/img/exitlag.png"></a>

Do you wanna create more than one trial account on exitlag? So this 'spoofer' is for you!

## Why? 

Exitlag limit the trial account creation to ONE account per HWID. You don't suppost to create more than one trial account with the same pc, because they get the HWID when you do login on their program and store it.
So what this program do is bypass this protection.

## How to use?
- Compile (or download the released version) and execute this program. It will execute exitlag.
- Login with a new account (create one with a tempemail, such has <a href="https://www.developermail.com/mail/" target="_blank">this one</a> or  <a href="https://www.gmailnator.com/" target="_blank">this one</a>).
- Logout you account. Login again. (without close exitlag)
- You gonna see that there are 3 days of trial.
Yes, you need to do this with every new account.
    
## How it works?
  - We hook some place in the assembly before it send the login request
  - Then we change the HWID to some other (rand letters)
  When the request is made to the server, the server will think it is a new pc and it will give us 3 days of trial.		

## This may help you

 - I made a <a href="https://greasyfork.org/pt-BR/scripts/420383-automatic-create-account-exitlag">greasyfork script</a> to automatically fill the fields of email and password. 
 - So, with one click you can create a new account.
 - I just didn't make a thing to create tons of accounts because they have a google captcha.

## Important
 - This program DO NOT change your hardware ID (hwid). I just edit the memory of the exitlag, so there is no way of your PC get problems by running this program.

## How to download?
 - https://github.com/bwmsdroid/exitlag-hwid-bypass/releases/

## TODO
 - clean the code
