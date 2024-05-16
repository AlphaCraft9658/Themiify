# Themiify
A new WIP theme manager for the Wii U console

(This theme manager will be started soon and is in very early stages, so it will not work for a while at all)

## Idea
This manager will be composed of both an app in the .wuhb format and a plugin. The app is for actually managing themes and selecting them and the plugin would load them dynamically. SDCafiine could potentially be used, but if we want to automatically apply the theme, SDCafiine would need to be set up to automatically load modpacks, if there is only one for the current title, which may not be the behavior the user wants, as it would apply to ALL titles and games. So a custom minimal plugin for loading the theme dynamically by replacing the theme files on the console on-the-fly is the approach. This plugin would utilize libredirect, which SDCafiine uses, so it should not be too hard to implement. This is just the concept for the beginning and things will be added one-by-one and carefully tested, but this is a rough explanation of the idea for how this should work.
