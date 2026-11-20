# Lua4RS Scripts

Maintained, useful scripts for [Lua4RS](https://github.com/sehraf/Lua4RS),
the Lua scripting plugin for RetroShare.

## buryTheDead.lua

Moves friends that have been offline for a long time into a separate group,
so the friend list stays readable.

- Friends whose last connection is at least `limit` days ago (default: 30) are
  assigned to the group `groupName` (default: `Graveyard`). The group is
  created if it does not exist yet.
- Friends in that group that have connected again are moved back out.
- `removeGroup()` deletes the group again, if you want to clean up.

Configure `groupName` and `limit` at the top of the script. Run it manually or
on a timer. Note: with many friends, running it may make the GUI lag briefly.

## syncFriends.lua

Keeps the friend lists of several of your own RetroShare nodes in sync, using
(distant) mail between your identities.

- Each node mails its **complete** friend list (a short invite for each
  location) as JSON to the identities in `SEND_TO`. Because every mail carries
  the full state, late or duplicate mails do no harm. Sequence numbers make
  sure older lists are ignored.
- Lists received from identities in `ACCEPT_FROM` are applied: missing friends
  are added and, if `REMOVE_FRIENDS` is enabled, friends the partner removed are
  removed. The first list from a new partner is answered with your own list.
- Processed sync mails are moved to the trash. Other mail is not touched.

### Setup

1. Edit the configuration at the top of the script:

   | Setting          | Default                     | Meaning                                         |
   |------------------|-----------------------------|-------------------------------------------------|
   | `SEND_AS`        | `"<own identity id>"`       | Your identity the mails are sent from           |
   | `SEND_TO`        | `{ "<partner identity id>" }` | Identities that receive your friend list      |
   | `ACCEPT_FROM`    | `{ "<partner identity id>" }` | Identities allowed to change your friends     |
   | `REMOVE_FRIENDS` | `false`                     | Also remove friends the partner removed         |
   | `RESEND_SECONDS` | `24 * 3600`                 | Send the list at least this often               |
   | `DRY_RUN`        | `true`                      | Only log which friends would be added/removed   |

   The placeholder IDs must be replaced with real identity IDs; otherwise
   nothing is sent and every incoming list is ignored.
2. Give the script write access (sandbox-write): it reads mail and invites and
   changes your friend list.
3. Set up the triggers: the events `mail_received` ("A mail arrives") and
   `friends_changed` ("A friend is added or removed"), a timer (e.g. every hour)
   and startup. In older plugin versions the friend-list event is called
   `list_changed`.
4. Check the log while `DRY_RUN` is enabled. Set it to `false` when you are
   happy with what the script would do.

### Testing

Running the script with `args.selftest = { from = ..., fromType = ..., message = ... }`
simulates receiving a message. It is always read-only and never sends mail.
