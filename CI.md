# CI/CD Workflow – R-Type

## Objectifs
- Build **cross-platform** (Linux + Windows)
- Tester le serveur et le client automatiquement
- Vérifier le style (clang-format)
- Cacher les dépendances pour ne pas dépasser le quota CI
- Déployer automatiquement le serveur sous forme de conteneur Docker

## Étapes principales
1. **PR Build** : compilation rapide + tests unitaires + format check
2. **Merge to main** : compilation complète (Linux + Windows)
3. **Tag vX.Y.Z** : build release + push image Docker + artefacts binaires
4. **Caches** :
    - `~/.ccache` pour compilation incrémentale
    - `~/.cache/vcpkg` ou `~/.conan/data` pour libs
5. **Qualité** :
    - clang-format sur chaque PR
    - clang-tidy en job optionnel (nightly)

## Commandes utiles
- `./ci/build.sh` : build local
- `./ci/test.sh` : run tests
- `./ci/check_clang_format.sh` : vérifier le style
- Pour forcer un refresh de cache : changer le hash dans `ci.yml`
