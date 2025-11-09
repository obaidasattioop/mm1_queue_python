# Login to instance
 ssh -i C:\Users\Obaid\OneDrive\Desktop\projects\iobnt_task\mm1-queue\obaid.pem ec2-user@ec2-3-86-16-62.compute-1.amazonaws.com

# Where is your code stored
cd /home/ec2-user/obaid/code

# Where are my plots stored (with timestamp)
cd /home/ec2-user/obaid/plots

# Activate virtual env on instance
cd /home/ec2-user/obaid
source venv/bin/activate

# Generate plots
cd /home/ec2-user/obaid/code
python mm1_queue.py

# Sync plots from remote to local (run on local machine)
scp -r -i C:\Users\Obaid\OneDrive\Desktop\projects\iobnt_task\mm1-queue\obaid.pem ec2-user@ec2-3-86-16-62.compute-1.amazonaws.com:/home/ec2-user/obaid/plots/. C:\Users\Obaid\OneDrive\Desktop\projects\iobnt_task\mm1-queue\plots\

# Sync code from local to remote (run on local machine)
scp -r -i C:\Users\Obaid\OneDrive\Desktop\projects\iobnt_task\mm1-queue\obaid.pem C:\Users\Obaid\OneDrive\Desktop\projects\iobnt_task\mm1-queue\src\. ec2-user@ec2-3-86-16-62.compute-1.amazonaws.com:/home/ec2-user/obaid/code


