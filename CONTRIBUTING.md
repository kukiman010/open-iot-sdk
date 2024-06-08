## Contributing

## License details

Please use an [SPDX license identifier](http://spdx.org/licenses/) in every
source file following the
[recommendations](https://spdx.dev/spdx-specification-21-web-version/#h.twlc0ztnng3b)
to make it easier for users to understand and review licenses.

The full original [Apache 2.0 license
text](http://www.apache.org/licenses/LICENSE-2.0) is in
[LICENSE-apache-2.0.txt](LICENSE-apache-2.0.txt). Each source file should start
with your copyright line followed by the SPDX identifier as shown here:

```
/* Copyright (c) 2013-2021, Arm  Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */
```

Each components part of the project must be listed in `LICENSE.md` file.

Please use the following Gitlab `json:table` feature:

```
    ```json:table
    {
        "fields":[
            "Component",
            "Path",
            "License",
            "Origin",
            "Category",
            "Version",
            "Security risk"
        ],
        "items":[
            {
                "Component":"",
                "Path":"",
                "License":"",
                "Origin":"",
                "Category":"",
                "Version":"",
                "Security risk":""
            }
        ]
    }
    ```
```

- `Component`: full component's name
- `Path`: relative path within the project. If multiple paths, use `,` as separator
- `License`: SPDX identifier
- `Origin`: url-origin
- `Category`:

    | Level  | Dependency        | Description                                      |
    | ------ | ----------------- | ------------------------------------------------ |
    | 1      | Source dependency | Redistributed: Build time – where we include 3rd party software in source within our distributed source |
    | 2      | Source dependency | Non-redistributed: Build time – where we do not redistribute 3rd party software in source but we rely on fetching and building that source |
    | 3      | Binary dependency | Deployment environment: Installation time – where we might have some dependencies which have to be satisfied on the deployment environment as part of an explicit installation step where we have ability to manage. |
    | 4      | Binary dependency | Deployment environment: Run time – where we might have some dependencies which have to be satisfied on the deployment environment implicitly due to chained dependencies where we have no ability to manage. |

- `Version` - tag or commit SHA
- `Security risk`- low/high - rating the influence of a component for a vulnerability risk

## Developer’s Certificate of Origin

Each commit must have at least one `Signed-off-by:` line from the committer to
certify that the contribution is made under the terms of the [Developer
Certificate of Origin](./DCO.txt).

If you set your `user.name` and `user.email` as part of your git configuration,
you can add a signoff to your commit automatically with `git commit -s`. You
have to use your real name (i.e., pseudonyms or anonymous contributions cannot
be made). This is because the DCO is a legally binding document.

## Maintainers and code owners

All contributions are ultimately merged by the maintainers listed below.
Technical ownership of most parts of the codebase falls on the code owners
listed in [CODEOWNERS](CODEOWNERS) file. An acknowledgement from these code
owners is required before the maintainers merge a contribution.

## Pre-commit

After you clone this project locally, we recommend installing [`pre-commit`](https://pre-commit.com/) and running

```
$ pre-commit install
```

This adds a pre-commit hook `.git/hooks/pre-commit` which gets run every time you create a git commit. It makes changes to your commit as needed such as formatting code and license headers.

Maintainers
===========

Martin Kojtal
    :email: `Martin Kojtal <Martin.Kojtal@arm.com>`
    :gitlab: `https://gitlab.arm.com/Martin.Kojtal`

Jaeden Amero
    :email: `Jaeden Amero <Jaeden.Amero@arm.com>`
    :gitlab: `https://gitlab.arm.com/Jaeden.Amero`
