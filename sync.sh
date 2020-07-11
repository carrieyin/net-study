while [ "true" ]; do
    echo '----------------------------------------------------------------------------'
    fswatch -r -L -1 *
    date
    rsync -r * rdtfare@10.221.155.87:/home/rdtfare/rdydd/ep
done
