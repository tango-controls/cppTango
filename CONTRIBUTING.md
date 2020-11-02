Thanks for your interest in contributing to cppTango.

# From idea to code

In general it is a good idea, to discuss additions and changes first in an
issue with the existing developers. This avoids duplicated work,
disappointments and can save you a lot of time. Maybe something close to what
you need is already there, and can be reused for your change.

We don't need a signed CLA (contributors license agreement) from you.

# Contribution steps

- Fork the repository to your own user
- Add your fork as new remote: `git remote add myFork git@github.com:<user>/cppTango.git`
- Create a new branch for your work
- Start hacking
- Create a pull request with your changes

Your fixes should always be based on the default branch `tango-9-lts`. Only
after accepting a PR against that branch, we can start integrating a fix for
the current stable version in the `9.3-backports` branch.

# Pull request acceptance and merging

You have created a change to cppTango. 🎉

And now you want to get these changes merged? Very nice!

In order to give you the best possible experience here are a few hints for the
path forward:
- All CI tests have to pass. If you have changed the behaviour of the code, you
  should add new tests as well. You don't need to execute the tests locally,
  CI is the reference anyway. So just create a PR and let CI handle that.
- Make your PR easy to review. This starts with explaining what it wants to
  achieve and ends with splitting the changes into logical commits where each
  commit describes why it is changing the code.
- Follow the coding style. This is at the moment messy at best, but still
  we don't want to get worse.
- Your PR needs two review approvals, including one from the code owners listed
  [here](https://github.com/tango-controls/cppTango/blob/tango-9-lts/CODEOWNERS).
- Be prepared to adapt your pull request to the review responses. Code review
  is done for ensuring higher code quality and communicating implementations
  details to newcomers and not for annoying anyone or slowing down development.
- Adapting existing pull requests also involves force-pushing new versions as
  we don't want to have intermediate versions in the history.
