# HobbitArmy

**A C++ assignment in data structures and algorithms**

> A peaceful home is worth defending. This project models the hobbits who stand together to protect theirs.

## The Battle of Bywater

It was a cold morning on November 3, 1420, by the Shire calendar. Fog hung low over the fields, and the roads of the Shire fell silent, as if holding their breath. Hobbits gathered in the meadows near Bywater: men and women, young and old. The Shire lay in ruins, and there was no time left to wait.

At their head stood Meriadoc Brandybuck, with Peregrin Took at his side, both hardened by the War of the Ring. Around them stood hobbits determined to defend the homes that had once been filled with song and laughter. Now they gripped slings, staves, and pitchforks. What an unusual sight.

Merry and Pippin positioned their people along the slopes of the old quarry. As the first light of day fell on the wet grass, a quiet signal was given. Stones flew from the thickets, and before the intruders could recover, hobbits charged from every side. Saruman's men, stunned by the fury of the small folk, stumbled through the mud and fell back.

The hobbit women fought alongside their companions: they aimed true, tended the wounded, and called out commands when others hesitated. The battle was short but fierce. Nineteen hobbits fell, and seventy outsiders were left in the dust. Those who survived laid down their weapons or fled the Shire.

After the battle, Frodo walked among the wounded and ordered that there be no vengeance. “A land that defends itself must not take on the cruelty of its enemies,” he said quietly.

So ended the Battle of Bywater, the second and last battle in the history of the Shire. It was small by the standards of kings, but to the hobbits it meant more than all the wars in the world: on that day, men and women stood side by side and won back their home and the peace that had been taken from them.

## Assignment

Implement the `HobbitArmy` class with the following operations:

| Method | Behavior |
| --- | --- |
| `add(hobbit)` | Adds a hobbit. Returns `false` without changing the army if the name is already present or the hobbit's HP is not positive; otherwise returns `true`. |
| `erase(name)` | Removes and returns the named hobbit, or an empty `std::optional` if no such hobbit exists. |
| `stats(name)` | Returns the current state of the named hobbit, or an empty `std::optional` if no such hobbit exists. |
| `for_each(functor)` | Calls the functor for every hobbit in alphabetical order. This is a read-only operation, so the functor may receive each hobbit by `const` reference. |
| `enchant(first, last, hp, off, def)` | Applies the given stat changes to every hobbit whose name falls alphabetically within the inclusive range `[first, last]`. If `first > last`, makes no changes and returns `true`. |

### `enchant` and HP validation

If an enchantment would reduce any affected hobbit's HP to zero or below, the operation must fail atomically: no hobbit is changed, and the method returns `false`. Efficiently checking this condition is challenging, so the assignment provides the `CHECK_NEGATIVE_HP` boolean constant to control whether those failure cases are tested.

When `CHECK_NEGATIVE_HP` is `false`, tests are guaranteed not to require `enchant` to fail, in exchange for a scoring penalty. **In this implementation, `CHECK_NEGATIVE_HP` is currently `true`.**