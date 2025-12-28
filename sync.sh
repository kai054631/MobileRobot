#!binbash
git add .
git commit -m Auto-sync $(date)
git push
echo Code is now synced to GitHub!
sleep 2
