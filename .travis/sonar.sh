#!/usr/bin/env bash

set -e

SONAR_PROPERTIES_BASIC="-Dsonar.login=${SONAR_TOKEN}"

if [ "$TRAVIS_PULL_REQUEST" == "false" ]; then
    SONAR_PROPERTIES="$SONAR_PROPERTIES_BASIC"
else
    SONAR_PROPERTIES="$SONAR_PROPERTIES_BASIC -Dsonar.pullrequest.base=$TRAVIS_BRANCH -Dsonar.pullrequest.branch=$TRAVIS_PULL_REQUEST_BRANCH -Dsonar.pullrequest.key=$TRAVIS_PULL_REQUEST"
fi

git fetch --unshallow --quiet

# pass -X for verbose output
docker exec -w "/home/tango/src" cpp_tango /home/tango/sonar-scanner/bin/sonar-scanner $SONAR_PROPERTIES
