#!/usr/bin/env bash

set -e

if [ "$TRAVIS_PULL_REQUEST" == "false" ]; then
    SONAR_PROPERTIES=""
else
    SONAR_PROPERTIES="-Dsonar.pullrequest.base=$TRAVIS_BRANCH -Dsonar.pullrequest.branch=$TRAVIS_PULL_REQUEST_BRANCH -Dsonar.pullrequest.key=$TRAVIS_PULL_REQUEST"
fi

echo "Using SONAR_PROPERTIES=$SONAR_PROPERTIES"

git fetch --unshallow --quiet

docker exec -w "/home/tango/src" cpp_tango /home/tango/sonar-scanner/bin/sonar-scanner -x $SONAR_PROPERTIES
