# Various maintenance-related tips

## How to cut a new ski release

1. Update version in `configure.ac`.
2. Update `NEWS` file to mention highlights and set current release date.
3. Commit the changes: ``git commit -a -m "'RELEASE: $VERSION'"``
4. Tag new release: ``git tag -u 7A8057EF -s -m "'$VERSION'" v$VERSION``
5. Generate tarballs: ``./autogen.sh && ./configure && make dist``
6. Push the release commit and the tag: ``git push --tags origin master``
7. Create release pushed tag and upload built tarball via github UI.
